#include "audiochat.h"

AudioChat::AudioChat()
{

}

AudioChat::~AudioChat(){
    delete m_connector;
}

void AudioChat::joinRoom(int roomId){
    m_connector = new UdpNet(roomId);

    m_collector.start();
    m_player.setProvider(&m_synthesizer);
    m_player.start();
    m_synthesizer.start();
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
    // 1 采集到一个音频帧，交给UdpConnector，UdpConnector负责发送出去
    connect(&m_collector, &AudioCollector::sig_audioFrameReady, m_connector, &UdpNet::onAudioFrameReady);

    // 2 UdpConnector收到一个未静音的音频帧（'F'开头），交给AudioSynthesizer放入对应队列
    connect(m_connector, &UdpNet::SIG_oneMsgReady, &m_synthesizer, &AudioSynthesizer::onOneFrameIn);

    // 3 UdpConnector收到一个静音的音频帧（'f'开头），交给AudioSynthesizer记录此用户还在线
    connect(m_connector, &UdpNet::SIG_oneEmptyFrameReady, &m_synthesizer, &AudioSynthesizer::onOneEmptyMsgIn);

    // 4 AudioSynthesizer每隔一段时间发送一个合成了的音频帧，交给AudioPlayer播放音频帧，已改为：player主动向AudioSynthesizer索要

//    qDebug() << "connect(&m_synthesizer, &AudioSynthesizer::sig_oneFrameReady, &m_player, &AudioPlayer::onAudioFrameReady)"
//    << connect(&m_synthesizer, &AudioSynthesizer::sig_oneFrameReady, &m_player, &AudioPlayer::onAudioFrameReady);
//    <<connect(&m_synthesizer, SIGNAL(sig_oneFrameReady(AudioFrame)), &m_player, SLOT(onAudioFrameReady(AudioFrame)));


    // 信号传递

    // 输入音量变化
    connect(&m_collector, &AudioCollector::sig_audioVolumeReady, [this](double volume){
//        qDebug() << "input volum =" << volume;
        emit sig_collectorVolumeReady(volume);
    });
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

void AudioChat::setInputDevice(QAudioDeviceInfo info){
    this->m_collector.setInputDevice(info);
}