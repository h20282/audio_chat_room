#include "audioplayer.h"

#include "log/log.h"

AudioPlayer::AudioPlayer() {
    QAudioFormat format;
    format.setSampleRate(kAudioSamRate);
    format.setSampleSize(kAudioSamSize);
    format.setChannelCount(kAudioSamCount);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    //    auto l = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    //    auto list = l[0].supportedCodecs();
    //    for (auto item : list) {
    //        qDebug() << item;
    //    }
    //    if (m_output != nullptr ) delete m_output;

    output_ = new QAudioOutput(format);
    audio_io_ = output_->start();
    playing_ = true;
}

AudioPlayer::~AudioPlayer() {

    playing_ = false;
    this->requestInterruption();
    QThread::msleep(100);
    audio_io_->close();
    output_->stop();
    this->quit();
    delete output_;
}

void AudioPlayer::run() {
    LOG_INFO("AudioPlayer start");
    auto maxFree = output_->bytesFree();
    while (!this->isInterruptionRequested() && playing_) {
        QMutexLocker locker(&mutex_);
        auto currBytesFree = output_->bytesFree();
        if (currBytesFree > maxFree) { maxFree = currBytesFree; }
        if (output_->bytesFree() >= kAudioFrameLen) {
            AudioFrame frame = m_provider->GetAudioFrame();
            if (frame.len < 0) {
                LOG_WARN("a wrong frame!(len({}) < 0)", frame.len);
                continue;
            } else if (frame.len == 0) {
                static int idx = 0;
                LOG_WARN("a empty frame!(len == 0)");
                QThread::msleep(50);
            } else {
                auto write_cnt = audio_io_->write(frame.buff, frame.len);
                LOG_INFO("audio player write {} bytes", write_cnt);
            }
        }
    }
    LOG_INFO("AudioPlayer end");
}

void AudioPlayer::SetProvider(AbstractAudioFrameProvider *provider) {
    m_provider = provider;
}
