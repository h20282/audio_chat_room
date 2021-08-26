#include "UdpConnector.h"

UdpConnector::UdpConnector(QString userName, int roomId)
    : m_roomId(roomId)
    , m_userName(userName)
    , m_port(UDP_SERVER_PORT)
{
#ifdef SAVE_ENCODE_IO_INTO_FILE
    fp_pcm = fopen("udp_net.pcm", "wb"); // todo: delete it!
    fp_aac = fopen("udp_net.aac", "wb");// todo: delete it!
#endif

#ifdef CODEC
    m_encoder = new Encoder(); // 获取编码器实例（初始化）
    initDecoder(); // 初始化解码器
#endif

    m_udpSocket = new QUdpSocket();
    unsigned short port = 0;
    while (not m_udpSocket->bind(QHostAddress::Any, port)){ //客户端接收消息的端口
        port++;
    }
    m_destaddr.setAddress(SERVER_IP);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &UdpConnector::onUdpReadyRead);
}

UdpConnector::~UdpConnector() {
#ifdef CODEC
    delete m_encoder; // 析构编码器（释放编码器资源）
    closeDecoder(); // 释放解码器资源

    for ( auto decoder : m_decoders ) {
        delete decoder;
    }
#endif

    m_udpSocket->close();
    delete m_udpSocket;
}

// 收到一个来自服务器的音频帧，交给AudioSynthesizer
void UdpConnector::onUdpReadyRead() {
    QMutexLocker locker(&m_mutex);
//    qDebug() << "onUdpReadyRead() start";
//    qDebug() << "UdpConnector::onUdpReadyRead()";
    while ( m_udpSocket->hasPendingDatagrams() ) {
        static char buff[sizeof(Msg)+128];
        auto recvLen = m_udpSocket->readDatagram(buff, sizeof(buff));
        qDebug() << "recvLen = " << recvLen;

        if (buff[0] == 'F') {

#ifdef CODEC

            QString name(buff + 1);

            int zipedFrameLen = *reinterpret_cast<int*>(buff+1+16);
            int recvIdx = *reinterpret_cast<int*>(buff+1+16);
//            qDebug () << "recv idx = " << recvIdx;

            // 从aac帧头获取aac帧长度
            unsigned char *headerBase = reinterpret_cast<unsigned char*> (buff + 1 + 16 + 4);
            int aacFrameLen = ( (headerBase[3]&0x03)<<(8+3) ) + ( headerBase[4]<<3 ) + (headerBase[5]>>5);

            if (headerBase[0]!=0xff){
                qDebug() << "not 0xff";
            }
            if (aacFrameLen != zipedFrameLen) {
                qDebug() << "aacFrameLen != zipedFrameLen" << aacFrameLen << zipedFrameLen << "end";
                continue;
            } else {
//                qDebug() << "aacFrameLen == zipedFrameLen" << aacFrameLen << zipedFrameLen << "end";
            }



//            auto pair = decodeFrame(buff+1+16+4, aacFrameLen);
            if (m_decoders.find(name) == m_decoders.end()){
                m_decoders[name] = new Decoder();
            }
            auto pair = m_decoders[name]->decodeFrame(buff+1+16+4, aacFrameLen);
            auto pcm_buff = pair.first;
            auto pcm_len = pair.second;


#ifdef SAVE_ENCODE_IO_INTO_FILE
            if (m_files.find(name) == m_files.end()) {
                qDebug() << "inner " << __LINE__;
                m_files.insert(name, fopen((name+".aac").toLatin1().data(), "wb"));
            }
            int flagHeaderLen = 1+16+4;
            qDebug() << "aac write to " << name << fwrite(buff+flagHeaderLen , 1, recvLen-flagHeaderLen , m_files[name]);
#endif
#ifdef SAVE_ENCODE_IO_INTO_FILE
            if (m_filesPCM.find(name) == m_filesPCM.end()) {
                qDebug() << "inner " << __LINE__;
                m_filesPCM.insert(name, fopen((name+".pcm").toLatin1().data(), "wb"));
            }
            qDebug() << "pcm write to " << name << fwrite(pcm_buff , 1, pcm_len , m_filesPCM[name]);
#endif

            if (pcm_len <= AUDIO_FRAME_LEN) { // 如果aac解压后的pcm长度小于一帧pcm的长度，直接发送
                Msg msg;
                memcpy(msg.name, buff+1, 16); // 用户名
                msg.frame.len = pcm_len; // pcm帧长度
                memcpy(msg.frame.buff, pcm_buff, pcm_len); // pcm数据
                emit sig_oneMsgReady(msg);
            } else { // 如果大于则拆分成多个
                for( int i=0; i<pcm_len; i+=AUDIO_FRAME_LEN) {
                    Msg msg;
                    memset(msg.frame.buff, 0, sizeof(msg.frame.buff)); // 全帧初始化为0
                    memcpy(msg.name, buff+1, 16); // 用户名
                    msg.frame.len = qMin(pcm_len-i, AUDIO_FRAME_LEN); // pcm帧长度
                    qDebug() << "in udp connector: msg.frame.len" << msg.frame.len;
                    memcpy(msg.frame.buff, pcm_buff+i, msg.frame.len); // pcm数据
                    emit sig_oneMsgReady(msg);
                }
            }
            delete pcm_buff;
#endif

#ifndef CODEC
            qDebug() << "udp emit";
            emit sig_oneMsgReady(*reinterpret_cast<Msg*>(buff+1));
#endif

        } else if (buff[0] == 'f') {
            QString name(buff+1);
            emit sig_oneEmptyFrameReady(name);
        } else {
            qDebug("unkown type :%d[%c]", buff[0], buff[0]);
        }

    }
//    qDebug() << "onUdpReadyRead() end";
}

// 收到一个来自Collector的音频帧，加上头部消息：【'F'/'f' + roomId + userName】，发送给服务器
void UdpConnector::onAudioFrameReady(AudioFrame frame) {
#ifdef CODEC
    if (m_isMuted){
        char buff[1+4+16];
        memset(buff, 0, sizeof(buff));
        buff[0] = 'f';
        *reinterpret_cast<int *>(buff + 1) = m_roomId;
        memcpy(buff + 1 + 4, m_userName.toLatin1().data(), m_userName.size());
        m_udpSocket->writeDatagram(buff, 1 + 4 + 16, m_destaddr, m_port);
    } else {
        QMutexLocker locker(&m_mutex);
        m_encoder->pushAudioFrame(frame);

#ifdef SAVE_ENCODE_IO_INTO_FILE
        fwrite(frame.buff, 1, frame.len, fp_pcm);
#endif

        while(1){
            ZipedFrame *zipedFrame = m_encoder->getZipedFrame();
            if (zipedFrame==nullptr) {
                break;
            }

#ifdef SAVE_ENCODE_IO_INTO_FILE
            fwrite(zipedFrame->data, 1, zipedFrame->len, fp_aac); // todo: delete it!
#endif

            //     1     +    4   +   16  +     4     + AAC_FRAME_LEN
            // 静音标识符 + 房间号 + 用户名 + aac帧长度 + aac帧数据
            //                          （用于验证aac帧正确性）
            char buff[1+4+16+4+zipedFrame->len];
            memset(buff, 0, sizeof(buff));

            buff[0] = 'F'; //未静音标志
            *reinterpret_cast<int *>(buff + 1) = m_roomId; // 房间号
            memcpy(buff + 1 + 4, m_userName.toLatin1().data(), m_userName.size()); // 用户名
            *reinterpret_cast<int *>(buff + 1 + 4 + 16) = zipedFrame->len; // aac帧长度
            memcpy(buff + 1 + 4 + 16 + 4, zipedFrame->data, zipedFrame->len);

            m_udpSocket->writeDatagram(buff, m_isMuted ? (1 + 4 + 16) : sizeof(buff), m_destaddr, m_port);
        }
    }
#endif

#ifndef CODEC
    char buff[1+4+16+sizeof (AudioFrame)];
    memset(buff, 0, sizeof(buff));
    buff[0] = m_isMuted?'f':'F';
    *reinterpret_cast<int*>(buff+1) = m_roomId;
    memcpy(buff+1+4, m_userName.toLatin1().data(), m_userName.size());
    memcpy(buff+1+4+16, &frame, sizeof (AudioFrame));

    auto sendLen = m_udpSocket->writeDatagram(buff, m_isMuted ? (1+4+16) : sizeof(buff), m_destaddr, m_port);
    qDebug() << "sendLen" << sendLen;
#endif
}

void UdpConnector::changeMuteState() {
    m_isMuted = !m_isMuted;
}

bool UdpConnector::getIsMuted() {
    return m_isMuted;
}

void UdpConnector::setIsMuted(bool isMuted) {
    this->m_isMuted = isMuted;
}
