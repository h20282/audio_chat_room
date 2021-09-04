#include "audiocollector.h"

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

#ifdef SAVE_COLLECTED_PCM_INTO_FILE
    m_fp = fopen(COLLECTED_PCM_PATH, "wb");
#endif
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
    connect(inputDevice_, &QIODevice::readyRead, this, &AudioCollector::onReadyRead);
}

void AudioCollector::run() {
    inputDevice_ = input_->start();
    connect(inputDevice_, &QIODevice::readyRead, this, &AudioCollector::onReadyRead);
}

void AudioCollector::onReadyRead() {
    QMutexLocker locker(&mutex_);
    AudioFrame frame;
    auto len = inputDevice_->read(frame.buff, sizeof(frame.buff));
    frame.len = static_cast<int>(len);

#ifdef SAVE_COLLECTED_PCM_INTO_FILE
    fwrite(frame.buff, 1, frame.len, m_fp);
#endif
    emit SigAudioFrameReady(frame);
    emit SigAudioVolumeReady(frame.getMaxVolume());
}
