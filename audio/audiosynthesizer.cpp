#include "audiosynthesizer.h"

#include "log/log.h"

namespace {
double f(int x) {
    return (pow(1.0223, x) - 1) * (100 / 8.07478);
}
}  // namespace

AudioSynthesizer::AudioSynthesizer() {

    QObject::connect(&timer_, &QTimer::timeout, [this]() {
        emit SigUserListReady(GetUserList());
        emit SigUserIsMutedStatusReady(is_muted_);
    });
    timer_.start(300);
}

AudioSynthesizer::~AudioSynthesizer() {}

AudioFrame AudioSynthesizer::GetAudioFrame() {
    QMutexLocker locker(&mutex_);
    return this->Synthese();
}

// 从各个队列中获取数据，合成为一个音频帧
AudioFrame AudioSynthesizer::Synthese() {
    AudioFrame frame;
    memset(&frame, 0, sizeof(0));
    int maxFrameLen = 0;
    double n = 0;  // 权值累和
    static int v[kAudioFrameLen / 2];
    memset(v, 0, sizeof(v));
    if (queues_.size())
        for (auto iter = queues_.begin(); iter != queues_.end(); iter++) {
            const auto &name = iter.key();
            auto &queue = iter.value();
            if (volume_.find(name) == volume_.end()) {
                n = volume_[name] = 100;
            }

            if (queue.size()) {
                auto x = f(volume_[name]);  // 权值
                n += 100;
                auto curr_frame = queue.dequeue();
                if (curr_frame.len != kAudioFrameLen) {
//                    spdlog::error("curr_frame.len({}) != AUDIO_FRAME_LEN({})", curr_frame.len, AUDIO_FRAME_LEN);
                }
                if (curr_frame.len > kAudioFrameLen) { continue; }

                maxFrameLen = qMax(maxFrameLen, curr_frame.len);

                auto base_b = reinterpret_cast<short *>(&curr_frame.buff[0]);
                for (int i = 0; i < maxFrameLen / 2; i++) {
                    v[i] += static_cast<int>(base_b[i] * x);
                }
            }
        }
    if (n != 0) {
        auto base_a = reinterpret_cast<short *>(&frame.buff[0]);
        for (int i = 0; i < maxFrameLen / 2; i++) {
            if (abs(v[i] / n) > 32767) {
                base_a[i] = v[i]>=0 ? 32767 : -32768;
            } else {
                base_a[i] = static_cast<short>(v[i] / n);
            }
        }
        frame.len = maxFrameLen;
    }
    return frame;
}

// 获取在线用户列表，超过3s没信号的则会被忽略
QList<QString> AudioSynthesizer::GetUserList() {
    QList<QString> ret;
    for (auto iter = last_online_t_.begin(); iter != last_online_t_.end();
         iter++) {
        QString name = iter.key();
        auto lastTime = iter.value();
        if (time(nullptr) - lastTime < 2) {
            ret.push_back(name);
        }
    }
    return ret;
}

void AudioSynthesizer::SetVolume(QString name, int volume) {
    if (volume >= 0 && volume <= 200) { volume_[name] = volume; }
}

// 每当有一个消息来临时，记录“该用户此时有信号”、入队
void AudioSynthesizer::onOneFrameIn(Msg msg) {
    LOG_INFO("one msg from {}, len = {}", msg.name, msg.frame.len);

    QMutexLocker locker(&mutex_);
    QString name(msg.name);
    is_muted_[name] = false;

    queues_[name].enqueue(msg.frame);

    last_online_t_[name] = time(nullptr);
    if (queues_[name].size() > 10) {
        LOG_WARN("droped one frame");
        queues_[name].dequeue();
    }
    if (volume_.find(name) != volume_.end()) {
        emit SigUserVolumeReady(
                name, msg.frame.getMaxVolume() * volume_[name] / 200);
    }
}

// 一个'f'开头的静音消息
void AudioSynthesizer::onOneEmptyMsgIn(QString userName) {
    QMutexLocker locker(&mutex_);
    is_muted_[userName] = true;
    last_online_t_[userName] = time(nullptr);
}
