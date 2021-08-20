/*
 * @Author: FengYanBin
 * @Date: 2021-08-09 11:19:42
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-20 19:53:10
 * @Description: file content
 * @FilePath: \sql\net_api\udp_net.cpp
 */
#include "udp_net.h"

UdpNet::UdpNet() : port_(kUdpServerPort), m_roomId(0)
{
    m_encoder = new Encoder(); // 获取编码器实例（初始化）
    initDecoder();             // 初始化解码器
    InitNet();
    qDebug() << "UdpNet::UdpNet finished";
}

UdpNet::~UdpNet()
{
    delete m_encoder; // 析构编码器（释放编码器资源）
    closeDecoder();   // 释放解码器资源
    CloseNet();
}

bool UdpNet::InitNet()
{
    m_udpSocket = new QUdpSocket();
    qDebug() << "udp server port:" << port_ << " "
             << "server ip:" << kServerIp << endl;
    m_udpSocket->bind(QHostAddress::Any, port_);
    m_destaddr.setAddress(kServerIp);

    qDebug() << "udp连接成功！" << endl;
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpNet::onUdpReadyRead);
    return true;
}

void UdpNet::CloseNet()
{
    if (m_udpSocket)
        m_udpSocket->close();
    delete m_udpSocket;
}

bool UdpNet::SendData(QByteArray data, QString IP)
{
    if (m_udpSocket->writeDatagram(data, data.length(), QHostAddress(IP), port_))
    {
        return true;
    }
    return false;
}

bool UdpNet::SendBroadCast(QByteArray data)
{
    if (m_udpSocket->writeDatagram(data, data.length(), QHostAddress::Broadcast, port_))
        return true;
    return false;
}

void UdpNet::setRoomId(int room_id)
{
    m_roomId = room_id;
}

// 收到一个来自服务器的音频帧，交给AudioSynthesizer(合成声音)
void UdpNet::onUdpReadyRead()
{
    QMutexLocker locker(&m_mutex);

    while (m_udpSocket->hasPendingDatagrams())
    {
        static char buff[sizeof(Msg) + 128];
        auto recvLen = m_udpSocket->readDatagram(buff, sizeof(buff));
        qDebug() << "recvLen = " << recvLen << endl;

        //没有静音
        if (buff[0] == 'F')
        {
            QString name(buff + 1);
            //编码：
            int zipedFrameLen = *reinterpret_cast<int *>(buff + 1 + 16);

            // 从aac帧头获取aac帧长度
            unsigned char *headerBase = reinterpret_cast<unsigned char *>(buff + 1 + 16 + 4);
            int aacFrameLen = ((headerBase[3] & 0x03) << (8 + 3)) + (headerBase[4] << 3) + (headerBase[5] >> 5);

            if (headerBase[0] != 0xff)
            {
                qDebug() << "not 0xff";
            }
            if (aacFrameLen != zipedFrameLen)
            {
                qDebug() << "aacFrameLen != zipedFrameLen" << aacFrameLen << zipedFrameLen << "end";
                continue;
            }
            else
            {
                //                qDebug() << "aacFrameLen == zipedFrameLen" << aacFrameLen << zipedFrameLen << "end";
            }

            auto pair = decodeFrame(buff + 1 + 16 + 4, zipedFrameLen);
            auto pcm_buff = pair.first;
            auto pcm_len = pair.second;

            Msg msg;
            memcpy(msg.name, buff + 1, 16);            // 用户名
            msg.frame.len = pcm_len;                   // pcm帧长度
            memcpy(msg.frame.buff, pcm_buff, pcm_len); // pcm数据

            emit SIG_oneMsgReady(*reinterpret_cast<Msg *>(buff + 1));
            delete pcm_buff;
        }
        else if (buff[0] == 'f')
        { //静音
            QString name(buff + 1);
            emit SIG_oneEmptyFrameReady(name);
        }
        else
        {
            qDebug("unkown type :%d[%c]", buff[0], buff[0]);
        }
    }
}

// 收到一个来自Collector的音频帧，加上头部消息：【'F'/'f' + roomId + userName】，发送给服务器
//静音要求有两个，一个是主持人静音某人，这个人说话全体成员都听不到。这里的逻辑是自己有个静音标志，一旦收到被主持人静音信号，将标志设置为true，不再发送给服务器。因此所有用户都听不到这个人说话了。
//另一个是用户可以静音某个人的声音，这个考虑用户这里用一个set保存自己静音的所有用户。然后播放的时候判断一下是否在集合中。
void UdpNet::onAudioFrameReady(AudioFrame frame)
{
    if (m_isMuted)
    {
        char buff[1 + 4 + 16];
        memset(buff, 0, sizeof(buff));
        buff[0] = 'f';
        *reinterpret_cast<int *>(buff + 1) = m_roomId;
        memcpy(buff + 1 + 4, g_userName.toLatin1().data(), g_userName.size());
        m_udpSocket->writeDatagram(buff, m_isMuted ? (1 + 4 + 16) : sizeof(buff), m_destaddr, port_);
    }
    else
    {
        QMutexLocker locker(&m_mutex);
        m_encoder->pushAudioFrame(frame);
        while (1)
        {
            ZipedFrame *zipedFrame = m_encoder->getZipedFrame();
            if (zipedFrame == nullptr)
            {
                break;
            }

            //     1     +    4   +   16  +     4     + AAC_FRAME_LEN
            // 静音标识符 + 房间号 + 用户名 + aac帧长度 + aac帧数据
            char buff[1 + 4 + 16 + 4 + zipedFrame->len];
            memset(buff, 0, sizeof(buff));

            buff[0] = 'F';                                                         //未静音标志
            *reinterpret_cast<int *>(buff + 1) = m_roomId;                         // 房间号
            memcpy(buff + 1 + 4, g_userName.toLatin1().data(), g_userName.size()); // 用户名
            *reinterpret_cast<int *>(buff + 1 + 4 + 16) = zipedFrame->len;
            memcpy(buff + 1 + 4 + 16 + 4, zipedFrame->data, zipedFrame->len);

            m_udpSocket->writeDatagram(buff, m_isMuted ? (1 + 4 + 16) : sizeof(buff), m_destaddr, port_);
        }
    }

    //    //qDebug() << "收到音频" << endl;
    //    char buff[1 + 4 + 16 + sizeof(AudioFrame)];
    //    memset(buff, 0, sizeof(buff));
    //    //qDebug() << "我当前静音状态m_isMuted=" << m_isMuted << endl;
    //    buff[0] = m_isMuted ? 'f' : 'F';
    //    *reinterpret_cast<int *>(buff + 1) = m_roomId;
    //    memcpy(buff + 1 + 4, g_userName.toLatin1().data(), g_userName.size());
    //    memcpy(buff + 1 + 4 + 16, &frame, sizeof(AudioFrame));

    //    m_udpSocket->writeDatagram(buff, m_isMuted ? (1 + 4 + 16) : sizeof(buff), m_destaddr, kServerPort);
}

void UdpNet::changeMuteState()
{
    m_isMuted = !m_isMuted;
}

void UdpNet::MuteUser()
{
    if (m_isMuted == false)
        m_isMuted = true;
}

void UdpNet::UnMuteUser()
{
    if (m_isMuted == true)
        m_isMuted = false;
}

bool UdpNet::getIsMuted()
{
    return m_isMuted;
}
