
#include "audiocollector.h"

QAudioInput *AudioCollector::createAudioInput(QAudioDeviceInfo info)
{
    format.setSampleRate(AUDIO_SAM_RATE);
    format.setSampleSize(AUDIO_SAM_SIZE);
    format.setChannelCount(AUDIO_SAM_COUNT);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    //m_playState = state_play;
    return new QAudioInput(info, format);
}

AudioCollector::AudioCollector()
{
    m_input = createAudioInput();
}

AudioCollector::~AudioCollector()
{

    m_playState = state_stop;
    this->m_input->stop();
    this->requestInterruption();
    msleep(100);
    delete m_input;
    delete m_inputDevice;
    delete m_connector;
}

void AudioCollector::setInputDevice(QAudioDeviceInfo info)
{
    QMutexLocker locker(&m_mutex);
    this->m_input->stop();
    delete m_input;
    delete m_inputDevice;

    m_input = createAudioInput(info);
    m_inputDevice = m_input->start();
    connect(m_inputDevice, &QIODevice::readyRead, this, &AudioCollector::onReadyRead);
}

void AudioCollector::Init()
{
    m_connector = new UdpNet();

    //qDebug() << "???" << endl;
    //m_collector.start();
    this->start();
    m_player.setProvider(&m_synthesizer);
    m_player.start();
    m_synthesizer.start();
    m_playState = state_play;
    is_first_connect = true;
    /*      +---------+
                               |        3|
                4              v         | f
   AudioPlayer <-- AudioSynthesizer <--+ |
                                      2| |
                                     F | |		+---+
   AudioCollector -----------> UdpConnector --> |   |
                            1			^		|net|
                                        |		|   |
                                        +-------|   |
                                                +---+      */

    // 2 UdpConnector收到一个未静音的音频帧（'F'开头），交给AudioSynthesizer放入对应队列
    connect(m_connector, &UdpNet::SIG_oneMsgReady, &m_synthesizer, &AudioSynthesizer::onOneFrameIn);

    // 3 UdpConnector收到一个静音的音频帧（'f'开头），交给AudioSynthesizer记录此用户还在线
    connect(m_connector, &UdpNet::SIG_oneEmptyFrameReady, &m_synthesizer, &AudioSynthesizer::onOneEmptyMsgIn);

        // 4 AudioSynthesizer每隔一段时间发送一个合成了的音频帧，交给AudioPlayer播放音频帧，已改为：player主动向AudioSynthesizer索要

    //    qDebug() << "connect(&m_synthesizer, &AudioSynthesizer::sig_oneFrameReady, &m_player, &AudioPlayer::onAudioFrameReady)"
    //    << connect(&m_synthesizer, &AudioSynthesizer::sig_oneFrameReady, &m_player, &AudioPlayer::onAudioFrameReady);
    //    <<connect(&m_synthesizer, SIGNAL(sig_oneFrameReady(AudioFrame)), &m_player, SLOT(onAudioFrameReady(AudioFrame)));

        // 信号传递

        // 用户音量变化
        connect(&m_synthesizer, &AudioSynthesizer::sig_userVolumeReady, [this](QString name, double volume){
    //        qDebug() << "output volum =" << name << volume;
            emit sig_userVolumeReady(name, volume);
        });
        // 用户列表变化
        connect(&m_synthesizer, &AudioSynthesizer::sig_userListReady, [this](QList<QString> list){
    //        qDebug() << list;
            emit sig_userListReady(list);
        });
}

void AudioCollector::firstConnect()
{

}

void AudioCollector::setUdpRoomId(int room_id)
{
    m_connector->setRoomId(room_id);
}

void AudioCollector::run()
{
    m_inputDevice = m_input->start();
    connect(m_inputDevice, &QIODevice::readyRead, this, &AudioCollector::onReadyRead);
}

void AudioCollector::onReadyRead()
{
    //qDebug() << "asfasf" << endl;
    QMutexLocker locker(&m_mutex);
    AudioFrame frame;
    auto len = m_inputDevice->read(frame.buff, sizeof(frame.buff));
    frame.len = static_cast<int>(len);
    //TODO:编码处理
    emit sig_audioFrameReady(frame);
    emit sig_audioVolumeReady(frame.getMaxVolume());
}

void AudioCollector::PauseAudio()
{
    if (m_playState == state_play)
    {
        m_playState = state_pause;
        if (m_input)
            m_input->stop();
    }
}

void AudioCollector::ResumeAudio(QAudioDeviceInfo info)
{
    qDebug() << "ResumeAudio" << endl;
    if (m_playState == state_stop)
    {
        m_input = new QAudioInput(info, format, this);
        m_inputDevice = m_input->start();
    }
    else
    {
        if (m_input)
        {
            if (is_first_connect && info == QAudioDeviceInfo::defaultInputDevice())
            {
                qDebug() << "第一次！" << endl;
                m_inputDevice = m_input->start();
                connect(m_inputDevice, &QIODevice::readyRead, this, &AudioCollector::onReadyRead);
                is_first_connect = false;

            }
            else
                setInputDevice(info);
            //            delete m_input;
            //            m_input = new QAudioInput(info, format, this);
            //            m_inputDevice = m_input->start();
        }
    }
    m_playState = state_play;
    //采集到一个音频帧，交给UdpConnector，UdpConnector负责发送出去
    connect(this, &AudioCollector::sig_audioFrameReady, m_connector, &UdpNet::onAudioFrameReady);

    // 输入音量变化
    connect(this, &AudioCollector::sig_audioVolumeReady, [this](double volume)
            {
                //        qDebug() << "input volum =" << volume;
                emit sig_collectorVolumeReady(volume);
            });
}
