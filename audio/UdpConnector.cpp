#include "UdpConnector.h"

#include "log/log.h"

namespace {

constexpr int kUserNameLen = 16;
constexpr char kMutedFlag = 'f';
constexpr char kUnMutedFlag = 'F';

struct AuPackHeader {
    char mute_flag;
    int room_id;
    char user_name[kUserNameLen];
};
}  // namespace

UdpConnector::UdpConnector(QString user_name, int room_id)
        : port_(UDP_SERVER_PORT), user_name_(user_name), room_id_(room_id) {
    encoder_ = new Encoder();

    udp_socket_ = new QUdpSocket();
    udp_socket_->bind(QHostAddress::Any);
    destaddr_.setAddress(SERVER_IP);
    connect(udp_socket_, &QUdpSocket::readyRead, this,
            &UdpConnector::onUdpReadyRead);
}

UdpConnector::~UdpConnector() {
    delete encoder_;
    for (auto decoder : decoders_) { delete decoder; }

    udp_socket_->close();
    delete udp_socket_;
}

// 收到一个来自服务器的音频帧，交给AudioSynthesizer
void UdpConnector::onUdpReadyRead() {
    //    QMutexLocker locker(&mutex_);
    while (udp_socket_->hasPendingDatagrams()) {
        auto recv_len = udp_socket_->pendingDatagramSize();
        if (recv_buff.size() < static_cast<size_t>(recv_len)) {
            recv_buff.resize(static_cast<size_t>(recv_len));
        }
        udp_socket_->readDatagram(&recv_buff[0], recv_len);
        LOG_INFO("udp recv {} bytes", recv_len);
        if (static_cast<size_t>(recv_len) < sizeof(AuPackHeader)) { continue; }

        AuPackHeader *recv_pkt =
                reinterpret_cast<AuPackHeader *>(&recv_buff[0]);
        QString user_name = QString::fromUtf8(
                reinterpret_cast<char *>(&recv_pkt->user_name), 16);
        if (recv_pkt->mute_flag == kMutedFlag) {

            emit SigOneEmptyFrameReady(user_name);

        } else if (recv_pkt->mute_flag == kUnMutedFlag) {

            std::vector<char> aac_data(
                    static_cast<size_t>(recv_len) - sizeof(AuPackHeader), 0);
            memcpy(&aac_data[0], &recv_buff[sizeof(AuPackHeader)],
                   aac_data.size());

            unsigned char *aac_frame_base = reinterpret_cast<unsigned char *>(
                    &recv_buff[sizeof(AuPackHeader)]);
            int aac_len = ((aac_frame_base[3] & 0x03) << (8 + 3)) +
                          (aac_frame_base[4] << 3) + (aac_frame_base[5] >> 5);
            if (aac_frame_base[0] != 0xff) {
                LOG_ERROR("aac_data error: not 0xff!");
            }
            if (aac_data.size() != static_cast<size_t>(aac_len)) {
                LOG_ERROR("aac_data len error! ({}) != ({})", aac_data.size(),
                          aac_len);
                continue;
            }

            if (decoders_.find(user_name) == decoders_.end()) {
                decoders_[user_name] = new Decoder();
            }
            auto pcm_data =
                    decoders_[user_name]->DecodeFrame(aac_frame_base, aac_len);
            LOG_INFO("decoder : {} --> {} ", aac_len, pcm_data.size());
            emit SigOneMsgReady(user_name, pcm_data);
        } else {
            LOG_ERROR("unknown type({0:d})'{0:c}'", recv_pkt->mute_flag);
        }
    }
}

// 收到一个来自Collector的音频帧
// 加上头部消息AuPackHeader，发送给服务器
void UdpConnector::onAudioFrameReady(std::vector<char> frame) {
    AuPackHeader header;
    memset(&header, 0, sizeof(header));
    header.room_id = room_id_;
    auto str_arr = user_name_.toUtf8();
    LOG_INFO("send user_name : '{}' (len={})", str_arr.data(), str_arr.size());
    memcpy(&header.user_name, str_arr.data(),
           static_cast<size_t>(str_arr.size()));
    if (is_muted_) {
        header.mute_flag = kMutedFlag;

        udp_socket_->writeDatagram(reinterpret_cast<char *>(&header),
                                   sizeof(header), destaddr_, port_);
    } else {
        QMutexLocker locker(&mutex_);
        header.mute_flag = kUnMutedFlag;
        encoder_->PushAudioFrame(frame);

        while (1) {
            LOG_INFO("befor get ziped frame");
            std::vector<char> aac_data = encoder_->GetZipedFrame();
            if (aac_data.size() == 0) { break; }
            std::vector<char> send_buff(sizeof(header) + aac_data.size());
            memcpy(&send_buff[0], &header, sizeof(header));
            memcpy(&send_buff[sizeof(header)], &aac_data[0], aac_data.size());

            auto len = udp_socket_->writeDatagram(
                    &send_buff[0], static_cast<qint64>(send_buff.size()),
                    destaddr_, port_);
            LOG_INFO("udp send {} bytes", len);
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
