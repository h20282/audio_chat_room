#include "UdpConnector.h"

#include "log/log.h"

UdpConnector::UdpConnector(QString user_name, int room_id)
        : port_(UDP_SERVER_PORT), user_name_(user_name), room_id_(room_id) {
#ifdef SAVE_ENCODE_IO_INTO_FILE
    fp_pcm = fopen("udp_net.pcm", "wb");  // todo: delete it!
    fp_aac = fopen("udp_net.aac", "wb");  // todo: delete it!
#endif

#ifdef CODEC
    encoder_ = new Encoder();  // 获取编码器实例（初始化）
#endif

    udp_socket_ = new QUdpSocket();
    unsigned short port = 0;
    while (!udp_socket_->bind(QHostAddress::Any,
                              port)) {  //客户端接收消息的端口
        port++;
    }
    destaddr_.setAddress(SERVER_IP);
    connect(udp_socket_, &QUdpSocket::readyRead, this,
            &UdpConnector::onUdpReadyRead);
}

UdpConnector::~UdpConnector() {
#ifdef CODEC
    delete encoder_;  // 析构编码器（释放编码器资源）
    // 释放解码器资源
    for (auto decoder : decoders_) { delete decoder; }
#endif

    udp_socket_->close();
    delete udp_socket_;
}

// 收到一个来自服务器的音频帧，交给AudioSynthesizer
void UdpConnector::onUdpReadyRead() {
    QMutexLocker locker(&mutex_);
    //    qDebug() << "onUdpReadyRead() start";
    //    qDebug() << "UdpConnector::onUdpReadyRead()";
    while (udp_socket_->hasPendingDatagrams()) {
        static char buff[sizeof(Msg) + 128];
        auto recv_len = udp_socket_->readDatagram(buff, sizeof(buff));
        LOG_INFO("udp_socket_->readDatagram = {}", recv_len);

        if (buff[0] == 'F') {

#ifdef CODEC
            QString name(buff + 1);

            int ziped_frame_len = *reinterpret_cast<int *>(buff + 1 + 16);

            // 从aac帧头获取aac帧长度
            unsigned char *header_base =
                    reinterpret_cast<unsigned char *>(buff + 1 + 16 + 4);
            int aacFrameLen = ((header_base[3] & 0x03) << (8 + 3)) +
                              (header_base[4] << 3) + (header_base[5] >> 5);

            if (header_base[0] != 0xff) { qDebug() << "not 0xff"; }
            if (aacFrameLen != ziped_frame_len) {
                qDebug() << "aacFrameLen != ziped_frame_len" << aacFrameLen
                         << ziped_frame_len << "end";
                continue;
            } else {
                //                qDebug() << "aacFrameLen == ziped_frame_len"
                //                << aacFrameLen << ziped_frame_len << "end";
            }

            //            auto pair = decodeFrame(buff+1+16+4, aacFrameLen);
            if (decoders_.find(name) == decoders_.end()) {
                decoders_[name] = new Decoder();
            }
            auto pair = decoders_[name]->DecodeFrame(buff + 1 + 16 + 4,
                                                     aacFrameLen);
            auto pcm_buff = pair.first;
            auto pcm_len = pair.second;

#ifdef SAVE_ENCODE_IO_INTO_FILE
            if (m_files.find(name) == m_files.end()) {
                qDebug() << "inner " << __LINE__;
                m_files.insert(name,
                               fopen((name + ".aac").toLatin1().data(), "wb"));
            }
            int flagHeaderLen = 1 + 16 + 4;
            qDebug() << "aac write to " << name
                     << fwrite(buff + flagHeaderLen, 1,
                               recv_len - flagHeaderLen, m_files[name]);
#endif
#ifdef SAVE_ENCODE_IO_INTO_FILE
            if (m_filesPCM.find(name) == m_filesPCM.end()) {
                qDebug() << "inner " << __LINE__;
                m_filesPCM.insert(
                        name, fopen((name + ".pcm").toLatin1().data(), "wb"));
            }
            qDebug() << "pcm write to " << name
                     << fwrite(pcm_buff, 1, pcm_len, m_filesPCM[name]);
#endif

            // 如果aac解压后的pcm长度小于一帧pcm的长度，直接发送
            if (pcm_len <= AUDIO_FRAME_LEN) {
                Msg msg;
                memcpy(msg.name, buff + 1, 16);
                msg.frame.len = pcm_len;
                memcpy(msg.frame.buff, pcm_buff, static_cast<size_t>(pcm_len));
                emit sig_oneMsgReady(msg);
            } else {  // 如果大于则拆分成多个
                for (int i = 0; i < pcm_len; i += AUDIO_FRAME_LEN) {
                    Msg msg;
                    // 全帧初始化为0
                    memset(msg.frame.buff, 0, sizeof(msg.frame.buff));
                    memcpy(msg.name, buff + 1, 16);  // 用户名
                    // pcm帧长度
                    msg.frame.len = qMin(pcm_len - i, AUDIO_FRAME_LEN);
                    LOG_INFO("in udp connector: msg.frame.len{}",
                             msg.frame.len);
                    // pcm数据
                    memcpy(msg.frame.buff, pcm_buff + i,
                           static_cast<size_t>(msg.frame.len));
                    emit sig_oneMsgReady(msg);
                }
            }
            delete pcm_buff;
#endif

#ifndef CODEC
            qDebug() << "udp emit";
            emit sig_oneMsgReady(*reinterpret_cast<Msg *>(buff + 1));
#endif

        } else if (buff[0] == 'f') {
            QString name(buff + 1);
            emit sig_oneEmptyFrameReady(name);
        } else {
            qDebug("unkown type :%d[%c]", buff[0], buff[0]);
        }
    }
    //    qDebug() << "onUdpReadyRead() end";
}

// 收到一个来自Collector的音频帧
//加上头部消息：【'F'/'f' + room_id + user_name】，发送给服务器
void UdpConnector::onAudioFrameReady(AudioFrame frame) {
#ifdef CODEC
    if (is_muted_) {
        char buff[1 + 4 + 16];
        memset(buff, 0, sizeof(buff));
        buff[0] = 'f';
        *reinterpret_cast<int *>(buff + 1) = room_id_;
        memcpy(buff + 1 + 4, user_name_.toLatin1().data(),
               static_cast<size_t>(user_name_.size()));
        udp_socket_->writeDatagram(buff, 1 + 4 + 16, destaddr_, port_);
    } else {
        QMutexLocker locker(&mutex_);
        encoder_->PushAudioFrame(frame);

#ifdef SAVE_ENCODE_IO_INTO_FILE
        fwrite(frame.buff, 1, frame.len, fp_pcm);
#endif

        while (1) {
            ZipedFrame *zipedFrame = encoder_->GetZipedFrame();
            if (zipedFrame == nullptr) { break; }

#ifdef SAVE_ENCODE_IO_INTO_FILE
            fwrite(zipedFrame->data, 1, zipedFrame->len,
                   fp_aac);  // todo: delete it!
#endif

            //     1     +    4   +   16  +     4     + AAC_FRAME_LEN
            // 静音标识符 + 房间号 + 用户名 + aac帧长度 + aac帧数据
            //                          （用于验证aac帧正确性）
            int buff_len = 1L + 4 + 16 + 4 + zipedFrame->len;
            char *buff = new char[static_cast<unsigned long long>(buff_len)];
            memset(buff, 0, static_cast<unsigned long long>(buff_len));

            //未静音标志
            buff[0] = 'F';
            *reinterpret_cast<int *>(buff + 1) = room_id_;
            memcpy(buff + 1 + 4, user_name_.toLatin1().data(),
                   static_cast<size_t>(user_name_.size()));
            *reinterpret_cast<int *>(buff + 1 + 4 + 16) = zipedFrame->len;
            memcpy(buff + 1 + 4 + 16 + 4, zipedFrame->data, static_cast<size_t>(zipedFrame->len));

            auto len = udp_socket_->writeDatagram(
                    buff, is_muted_ ? (1 + 4 + 16) : buff_len, destaddr_,
                    port_);
            LOG_INFO("udp_socket_->writeDatagram = {}", len);

            //            spdlog::info("file[{}] func[{}]
            //            line[{}]:udp_socket_->writeDatagram = {}", __FILE__,
            //            __func__, __LINE__ , len);
            delete[] buff;
        }
    }
#endif

#ifndef CODEC
    char buff[1 + 4 + 16 + sizeof(AudioFrame)];
    memset(buff, 0, sizeof(buff));
    buff[0] = m_isMuted ? 'f' : 'F';
    *reinterpret_cast<int *>(buff + 1) = m_room_id;
    memcpy(buff + 1 + 4, m_user_name.toLatin1().data(), m_user_name.size());
    memcpy(buff + 1 + 4 + 16, &frame, sizeof(AudioFrame));

    auto sendLen = m_udpSocket->writeDatagram(
            buff, m_isMuted ? (1 + 4 + 16) : sizeof(buff), m_destaddr, m_port);
    qDebug() << "sendLen" << sendLen;
#endif
}

void UdpConnector::ChangeMuteState() {
    is_muted_ = !is_muted_;
}

bool UdpConnector::GetIsMuted() {
    return is_muted_;
}

void UdpConnector::SetIsMuted(bool isMuted) {
    this->is_muted_ = isMuted;
}
