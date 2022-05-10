#include "audiocollector.h"

#include "log/log.h"

namespace {
static QAudioInput *createAudioInput(
        QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice()) {
    QAudioFormat format;
    format.setSampleRate(kAudioSamRate);
    format.setSampleSize(kAudioSamSize);
    format.setChannelCount(kAudioSamCount);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    return new QAudioInput(info, format);
}
}  // namespace

AudioCollector::AudioCollector() {
    input_ = createAudioInput();
}

AudioCollector::~AudioCollector() {
    this->input_->stop();
    delete input_;
    delete inputDevice_;
}

void AudioCollector::SetInputDevice(QAudioDeviceInfo info) {
    QMutexLocker locker(&mutex_);
    this->input_->stop();
    delete input_;
    delete inputDevice_;

    input_ = createAudioInput(info);
    inputDevice_ = input_->start();
    connect(inputDevice_, &QIODevice::readyRead, this,
            &AudioCollector::onReadyRead);
}

void AudioCollector::Start() {
    LOG_INFO("start");
    inputDevice_ = input_->start();
    connect(inputDevice_, &QIODevice::readyRead, this,
            &AudioCollector::onReadyRead);
}

void AudioCollector::onReadyRead() {
    QMutexLocker locker(&mutex_);

    codec::AudioData pcm_data = std::make_shared<std::vector<uint8_t>>(
            static_cast<std::size_t>(input_->bytesReady()));
    if (pcm_data->size() == 0) { return; }

    inputDevice_->read(reinterpret_cast<char *>(pcm_data->data()),
                       static_cast<qint64>(pcm_data->size()));
    LOG_PER(50, "{} bytes ready", pcm_data->size());
    emit SigAudioFrameReady(pcm_data);
    // get max volume:
    short *p = reinterpret_cast<short *>(pcm_data->data());
    auto max_volume = *std::max_element(p, p + pcm_data->size() / 2);
    auto vol = static_cast<double>(max_volume) / 32768;
    emit SigAudioVolumeReady(vol);
}
