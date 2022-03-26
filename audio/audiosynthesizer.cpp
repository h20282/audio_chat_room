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

std::vector<char> AudioSynthesizer::GetAudioFrame() {
    QMutexLocker locker(&mutex_);
    return this->Synthese();
}

// 从各个队列中获取数据，合成为一个音频帧
std::vector<char> AudioSynthesizer::Synthese() {
    bool has_data = false;
    for (const auto &queue : queues_) {
        if (queue.size()) {
            has_data = true;
            break;
        }
    }
    if (!has_data) { return {}; }

    std::size_t max_size = 0;
    for (auto &queue : queues_) {
        if (queue.size()) {
            max_size = std::max(max_size, queue.back()->size());
        }
    }
    if (max_size == 0) { return {}; }
    LOG_ERROR("max_size = {}", max_size);
    std::vector<char> synthesed_data(max_size, 0);
    double n = 0;
    std::vector<double> au_data(synthesed_data.size() / 2, 0);

    for (auto iter = queues_.begin(); iter != queues_.end(); ++iter) {
        const auto &name = iter.key();
        auto &queue = iter.value();
        if (volume_.find(name) == volume_.end()) { n = volume_[name] = 100; }

        if (queue.size()) {
            double x = f(volume_[name]);  // 权值
            n += 100;
            auto curr_frame = queue.dequeue();

            auto base_b = reinterpret_cast<short *>(curr_frame->data());
            for (std::size_t i = 0; i < au_data.size(); ++i) {
                au_data[i] += base_b[i] * x;
            }
        }
    }

    auto base = reinterpret_cast<short *>(&synthesed_data[0]);
    for (std::size_t i = 0; i < au_data.size(); ++i) {
        double amp = au_data[i] / n;
        base[i] = static_cast<short>(amp);
        if (amp > 32767) { base[i] = 32767; }
        if (amp < -32768) { base[i] = -32768; }
    }

    return synthesed_data;
}

// 获取在线用户列表，超过2s没信号的则会被忽略
QList<QString> AudioSynthesizer::GetUserList() {
    QList<QString> ret;
    for (auto iter = last_online_t_.begin(); iter != last_online_t_.end();
         ++iter) {
        QString name = iter.key();
        auto lastTime = iter.value();
        if (time(nullptr) - lastTime < 2) { ret.push_back(name); }
    }
    return ret;
}

void AudioSynthesizer::SetVolume(QString name, int volume) {
    if (volume >= 0 && volume <= 200) { volume_[name] = volume; }
}

// 每当有一个消息来临时，记录“该用户此时有信号”、入队
void AudioSynthesizer::onOneFrameIn(QString name, codec::AudioData pcm_data) {
    LOG_INFO("one msg from {}, len = {}", name.toUtf8().data(),
             pcm_data->size());
    QMutexLocker locker(&mutex_);

    is_muted_[name] = false;
    queues_[name].enqueue(pcm_data);
    last_online_t_[name] = time(nullptr);
    if (queues_[name].size() > 9) {
        LOG_WARN("droped one frame");
        queues_[name].dequeue();
    }

    // get max volume:
    short *p = reinterpret_cast<short *>(pcm_data->data());
    auto max_volume = *std::max_element(p, p + pcm_data->size() / 2);
    if (volume_.find(name) == volume_.end()) { volume_[name] = 100; }
    auto vol = static_cast<double>(max_volume) / 32768 * volume_[name] / 200;
    if (volume_.find(name) != volume_.end()) {
        emit SigUserVolumeReady(name, vol);
    }
}

void AudioSynthesizer::onOneEmptyMsgIn(QString userName) {
    QMutexLocker locker(&mutex_);
    is_muted_[userName] = true;
    last_online_t_[userName] = time(nullptr);
}
