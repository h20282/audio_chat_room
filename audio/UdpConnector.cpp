#include "UdpConnector.h"

#include "log/log.h"

// clang-format off
namespace {
// client --> server
//     1     +    4   +   16  +     4     + AAC_FRAME_LEN
// 静音标识符 + 房间号 + 用户名 + aac帧长度 + aac帧数据
constexpr char kUnMutedFlag = 'F';
constexpr char kMutedFlag = 'f';

constexpr int kMuteLen = 1;
constexpr int kRoomIdLen = 4;
constexpr int kUserNameLen = 16;
constexpr int kAacLen = 4;


constexpr int kClientToServerHeaderLen  = 0 + kMuteLen + kRoomIdLen + kUserNameLen + kAacLen;

constexpr int kClientToSererMutePos     = 0;
constexpr int kClientToSererRoomIdPos   = 0 + kMuteLen;
constexpr int kClientToSererUserNamePos = 0 + kMuteLen + kRoomIdLen;
constexpr int kClientToSererAacLenPos   = 0 + kMuteLen + kRoomIdLen + kUserNameLen;
constexpr int kClientToSererAacDataPos  = 0 + kMuteLen + kRoomIdLen + kUserNameLen + kAacLen;

// server --> client
//     1     +   16  +     4     + AAC_FRAME_LEN
// 静音标识符 + 用户名 + aac帧长度 + aac帧数据
constexpr int kServerToClientHeaderLen   = 0 + kMuteLen + kUserNameLen + kAacLen;

constexpr int kServerToClientMutePos     = 0;
constexpr int kServerToClientUserNamePos = 0 + kMuteLen;
constexpr int kServerToClientAacLenPos   = 0 + kMuteLen + kUserNameLen;
constexpr int kServerToClientAacDataPos  = 0 + kMuteLen + kUserNameLen + kAacLen;
}  // namespace
// clang-format on

UdpConnector::UdpConnector(QString user_name, int room_id)
        : port_(UDP_SERVER_PORT), user_name_(user_name), room_id_(room_id) {
    encoder_ = new Encoder();  // 获取编码器实例（初始化）

    udp_socket_ = new QUdpSocket();
    udp_socket_->bind(QHostAddress::Any);
    destaddr_.setAddress(SERVER_IP);
    connect(udp_socket_, &QUdpSocket::readyRead, this,
            &UdpConnector::onUdpReadyRead);
}

UdpConnector::~UdpConnector() {
    delete encoder_;  // 析构编码器（释放编码器资源）
    // 释放解码器资源
    for (auto decoder : decoders_) { delete decoder; }

    udp_socket_->close();
    delete udp_socket_;
}

// 收到一个来自服务器的音频帧，交给AudioSynthesizer
void UdpConnector::onUdpReadyRead() {
    QMutexLocker locker(&mutex_);
    static char buff[sizeof(Msg) + 128];
    while (udp_socket_->hasPendingDatagrams()) {
        auto recv_len = udp_socket_->readDatagram(buff, sizeof(buff));
        LOG_INFO("udp_socket_->readDatagram = {}", recv_len);

        if (buff[0] == 'F') {

            QString name(buff + 1);

            int ziped_frame_len = *reinterpret_cast<int *>(buff + 1 + 16);

            // 从aac帧头获取aac帧长度
            unsigned char *header_base =
                    reinterpret_cast<unsigned char *>(buff + 1 + 16 + 4);
            int aac_len = ((header_base[3] & 0x03) << (8 + 3)) +
                              (header_base[4] << 3) + (header_base[5] >> 5);

            if (header_base[0] != 0xff) {
                LOG_ERROR("not 0xff");
                continue;
            }
            if (aac_len != ziped_frame_len) {
                LOG_ERROR("aac_len:{} != ziped_frame_len:{}", aac_len,
                          ziped_frame_len);
                continue;
            }
            if (decoders_.find(name) == decoders_.end()) {
                decoders_[name] = new Decoder();
            }
            auto pair = decoders_[name]->DecodeFrame(buff + 1 + 16 + 4,
                                                     aac_len);
            auto pcm_buff = pair.first;
            auto pcm_len = pair.second;

            // 如果aac解压后的pcm长度小于一帧pcm的长度，直接发送
            if (pcm_len <= kAudioFrameLen) {
                Msg msg;
                memcpy(msg.name, buff + 1, 16);
                msg.frame.len = pcm_len;
                memcpy(msg.frame.buff, pcm_buff, static_cast<size_t>(pcm_len));
                emit SigOneMsgReady(msg);
            } else {  // 如果大于则拆分成多个
                for (int i = 0; i < pcm_len; i += kAudioFrameLen) {
                    Msg msg;
                    // 全帧初始化为0
                    memset(msg.frame.buff, 0, sizeof(msg.frame.buff));
                    memcpy(msg.name, buff + 1, 16);  // 用户名
                    // pcm帧长度
                    msg.frame.len = qMin(pcm_len - i, kAudioFrameLen);
                    LOG_INFO("in udp connector: msg.frame.len{}",
                             msg.frame.len);
                    // pcm数据
                    memcpy(msg.frame.buff, pcm_buff + i,
                           static_cast<size_t>(msg.frame.len));
                    emit SigOneMsgReady(msg);
                }
            }
            delete pcm_buff;

        } else if (buff[0] == 'f') {
            QString name(buff + 1);
            emit SigOneEmptyFrameReady(name);
        } else {
            qDebug("unkown type :%d[%c]", buff[0], buff[0]);
        }
    }
    //    qDebug() << "onUdpReadyRead() end";
}

// 收到一个来自Collector的音频帧
//加上头部消息：【'F'/'f' + room_id + user_name】，发送给服务器
void UdpConnector::onAudioFrameReady(AudioFrame frame) {
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

        while (1) {
            LOG_INFO("befor get ziped frame");
            std::vector<char> aac_data = encoder_->GetZipedFrame();
            LOG_INFO("ziped frame len = {}", aac_data.size());
            if (aac_data.size() == 0) { break; }

            //     1     +    4   +   16  +     4     + AAC_FRAME_LEN
            // 静音标识符 + 房间号 + 用户名 + aac帧长度 + aac帧数据
            //                          （用于验证aac帧正确性）
            std::size_t send_len = kClientToServerHeaderLen + aac_data.size();
            std::vector<char> send_buff(send_len, 0);
            LOG_INFO("send_buff.size = {}", send_buff.size());

            // 1.禁言标识符号
            send_buff[0] = kUnMutedFlag;

            // 2.房间号
            *reinterpret_cast<int *>(&send_buff[kClientToSererRoomIdPos]) =
                    room_id_;

            // 3.用户名（warning: 中文问题、长度问题）
            memcpy(&send_buff[kClientToSererUserNamePos],
                   user_name_.toLatin1().data(),
                   static_cast<size_t>(user_name_.size()));

            // 4.aac帧长度
            *reinterpret_cast<int *>(&send_buff[kClientToSererAacLenPos]) =
                    static_cast<int>(aac_data.size());

            // 5.aac帧数据
            memcpy(&send_buff[kClientToSererAacDataPos], &aac_data[0],
                   aac_data.size());

            auto len = udp_socket_->writeDatagram(
                    &send_buff[0], static_cast<qint64>(send_buff.size()),
                    destaddr_, port_);
            LOG_INFO("udp_socket_->writeDatagram = {}", len);
        }
    }
}

void UdpConnector::ToggleMute() {
    is_muted_ = !is_muted_;
}

bool UdpConnector::IsMuted() {
    return is_muted_;
}

void UdpConnector::SetMuted(bool isMuted) {
    this->is_muted_ = isMuted;
}
