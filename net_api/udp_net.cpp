/*
 * @Author: FengYanBin
 * @Date: 2021-08-09 11:19:42
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-20 15:42:54
 * @Description: file content
 * @FilePath: \sql\net_api\udp_net.cpp
 */
#include "udp_net.h"

UdpNet::UdpNet() : port_(kServerPort), m_roomId(0)
{
    InitNet();
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

    //    connect(m_chat_room, &MyChatRoom::SIG_isMuted, [this](){
    //        m_isMuted = true;
    //    });
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

        //没有静音
        if (buff[0] == 'F')
        {
            QString name(buff + 1);
            std::string str = name.toStdString();
            //TODO:用一个set保存自己静音的所有用户。然后播放的时候判断一下是否在集合中。将用户自己静音的用户声音不再播放。
            for (auto it = m_setMuteUsers.begin(); it != m_setMuteUsers.end(); ++it)
            {
                if ((*it) == str)
                    return;
            }
            emit SIG_oneMsgReady(*reinterpret_cast<Msg *>(buff + 1));
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
    //qDebug() << "收到音频" << endl;
    char buff[1 + 4 + 16 + sizeof(AudioFrame)];
    memset(buff, 0, sizeof(buff));
    //qDebug() << "我当前静音状态m_isMuted=" << m_isMuted << endl;
    buff[0] = m_isMuted ? 'f' : 'F';
    *reinterpret_cast<int *>(buff + 1) = m_roomId;
    memcpy(buff + 1 + 4, g_userName.toLatin1().data(), g_userName.size());
    memcpy(buff + 1 + 4 + 16, &frame, sizeof(AudioFrame));

    m_udpSocket->writeDatagram(buff, m_isMuted ? (1 + 4 + 16) : sizeof(buff), m_destaddr, kServerPort);
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

void UdpNet::insertMuteUser(std::string name)
{
    m_setMuteUsers.insert(name);
}

void UdpNet::delMuteUser(std::string name)
{
    m_setMuteUsers.erase(name);
}
