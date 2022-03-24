#include "UdpConnector.h"

#include "Config.h"
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
        : port_(UDP_SERVER_PORT),
          user_name_(user_name),
          room_id_(room_id),
          encoder_(kAudioSamRate, kAudioSamRate) {

    udp_socket_ = new QUdpSocket();
    udp_socket_->bind(QHostAddress::Any);
    destaddr_.setAddress(SERVER_IP);
    connect(udp_socket_, &QUdpSocket::readyRead, this,
            &UdpConnector::onUdpReadyRead);
}

UdpConnector::~UdpConnector() {
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

            uint8_t *aac_frame_base = reinterpret_cast<uint8_t *>(
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

            auto decoder = decoders_.find(user_name);
            if (decoder == decoders_.end()) {
                decoders_.insert(std::make_pair(
                        user_name, ODecoder(kAudioSamRate, kAudioSamCount)));
            }
            auto pcm_data =
                    decoders_.at(user_name).Decode(aac_frame_base, aac_len);
            LOG_INFO("decoder : {} --> {} ", aac_len, pcm_data->size());
            emit SigOneMsgReady(user_name, pcm_data);
        } else {
            LOG_ERROR("unknown type({0:d})'{0:c}'", recv_pkt->mute_flag);
        }
    }
}

// 收到一个来自Collector的音频帧
// 加上头部消息AuPackHeader，发送给服务器
void UdpConnector::onAudioFrameReady(AudioData frame) {
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

        auto encoded_data = encoder_.Encode(frame);
        uint8_t *beg = reinterpret_cast<uint8_t *>(&header);
        uint8_t *end =
                reinterpret_cast<uint8_t *>(&header + sizeof(AuPackHeader));
        std::vector<uint8_t> buffer(beg, end);
        buffer.assign(encoded_data->begin(), encoded_data->end());
        auto len = udp_socket_->writeDatagram(
                reinterpret_cast<char *>(buffer.data()), buffer.size(),
                destaddr_, port_);
        LOG_INFO("udp send {} bytes", len);
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
