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

void AudioCollector::run() {
    inputDevice_ = input_->start();
    connect(inputDevice_, &QIODevice::readyRead, this,
            &AudioCollector::onReadyRead);
}

void AudioCollector::onReadyRead() {
    QMutexLocker locker(&mutex_);

    std::vector<char> pcm_data(static_cast<std::size_t>(input_->bytesReady()));

    inputDevice_->read(reinterpret_cast<char *>(&pcm_data[0]),
                                  static_cast<qint64>(pcm_data.size()));
    emit SigAudioFrameReady(pcm_data);
    // get max volume:
    short *p = reinterpret_cast<short*>(&pcm_data[0]);
    auto max_volume = *std::max_element(p, p+pcm_data.size()/2);
    auto vol = static_cast<double>(max_volume) / 32768;
    emit SigAudioVolumeReady(vol);
}
