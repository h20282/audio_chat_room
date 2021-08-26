
#include "AudioCollector.h"

static QAudioInput *createAudioInput(QAudioDeviceInfo info=QAudioDeviceInfo::defaultInputDevice()){
    QAudioFormat format;
    format.setSampleRate(AUDIO_SAM_RATE);
    format.setSampleSize(AUDIO_SAM_SIZE);
    format.setChannelCount(AUDIO_SAM_COUNT);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    return new QAudioInput(info, format);
}

AudioCollector::AudioCollector(){
    m_input = createAudioInput();

#ifdef SAVE_COLLECTED_PCM_INTO_FILE
    m_fp = fopen(COLLECTED_PCM_PATH, "wb");
#endif
}

AudioCollector::~AudioCollector(){
    this->m_input->stop();
    delete m_input;
    delete m_inputDevice;
}

void AudioCollector::setInputDevice(QAudioDeviceInfo info){
    QMutexLocker locker(&m_mutex);
    this->m_input->stop();
    delete m_input;
    delete m_inputDevice;

    m_input = createAudioInput(info);
    m_inputDevice = m_input->start();
    connect(m_inputDevice, &QIODevice::readyRead, this, &AudioCollector::onReadyRead);
}

void AudioCollector::run(){
    m_inputDevice = m_input->start();
    connect(m_inputDevice, &QIODevice::readyRead, this, &AudioCollector::onReadyRead);
}

void AudioCollector::onReadyRead(){
    QMutexLocker locker(&m_mutex);
    AudioFrame frame;
    auto len = m_inputDevice->read(frame.buff, sizeof(frame.buff));
    frame.len = static_cast<int>(len);

#ifdef SAVE_COLLECTED_PCM_INTO_FILE
    fwrite(frame.buff, 1, frame.len, m_fp);
#endif
//    qDebug() << "curr volume:" << frame.getVolumeSum();
//    if (frame.getVolumeSum() < 0.002){
//        memset(frame.buff, 0, sizeof(frame.buff));
//        frame.len = 0;
//    }
    emit sig_audioFrameReady(frame);
    emit sig_audioVolumeReady(frame.getMaxVolume());
}

