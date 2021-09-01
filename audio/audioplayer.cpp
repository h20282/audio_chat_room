#include "AudioPlayer.h"

AudioPlayer::AudioPlayer() {
    QAudioFormat format;
    format.setSampleRate(AUDIO_SAM_RATE);
    format.setSampleSize(AUDIO_SAM_SIZE);
    format.setChannelCount(AUDIO_SAM_COUNT);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

//    auto l = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
//    auto list = l[0].supportedCodecs();
//    for (auto item : list) {
//        qDebug() << item;
//    }
//    if (m_output != nullptr ) delete m_output;

    m_output = new QAudioOutput(format);
    m_audioIo = m_output->start();
    m_playing = true;

}

AudioPlayer::~AudioPlayer() {

    m_playing = false;
    this->requestInterruption();
    QThread::msleep(100);
    m_audioIo->close();                         //qDebug() << __LINE__;
    m_output->stop();                           //qDebug() << __LINE__;
    this->quit();                               //qDebug() << __LINE__;
    delete m_output;                            //qDebug() << __LINE__;
}

void AudioPlayer::run() {
    qDebug() << "void AudioPlayer::run()";
    auto maxFree = m_output->bytesFree();
    while (!this->isInterruptionRequested() && m_playing) {
        QMutexLocker locker(&m_mutex);
        auto currBytesFree = m_output->bytesFree();
        if ( currBytesFree > maxFree ) {
            maxFree = currBytesFree;
        }
//        qDebug() << "currBytesFree =" << currBytesFree << ", maxFree = " << maxFree;
        if (m_output->bytesFree()>=sizeof(AudioFrame)){
            AudioFrame frame = m_provider->getAudioFrame();
            if (frame.len < 0) {
                qWarning() << "a wrong frame!(len < 0)";
                continue;
            } else if (frame.len==0){
                static int idx = 0;
                qWarning() << "a empty frame!(len == 0)" << ++idx;
                QThread::msleep(50);
            } else {
                auto writeCnt = m_audioIo->write(frame.buff, frame.len);
//                qDebug() << writeCnt << "bytes audio writen(audio player)";
            }

        }
    }
    qDebug() << "void AudioPlayer::run() end";
}

void AudioPlayer::setProvider(AbstractAudioFrameProvider *provider){
    m_provider = provider;
}


// 已弃用
//// 当接收到一个音频帧时，将其放入队列，队列内音频帧过多则舍弃1个旧的
//void AudioPlayer::onAudioFrameReady(AudioFrame frame){
//    QMutexLocker locker(&m_mutex);
//    m_queue.enqueue(frame);
//    if (m_queue.size()>100){
//        m_queue.dequeue();
//        qWarning() << "too much audio frame!";
//    }
//}
