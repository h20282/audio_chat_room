#include "AudioChat.h"

#include "log/log.h"

AudioChat::AudioChat() {}

AudioChat::~AudioChat() {
    delete connector_;
}

void AudioChat::JoinRoom(QString user_name, int room_id) {

    LOG_INFO("{} join room {}", user_name.toLatin1().data(), room_id);
    connector_ = new UdpConnector(user_name, room_id);

    collector_.start();
    player_.SetProvider(&synthesizer_);
    player_.start();

    // 1 采集到一个音频帧，交给UdpConnector，UdpConnector负责发送出去
    connect(&collector_, &AudioCollector::sig_audioFrameReady, connector_,
            &UdpConnector::onAudioFrameReady);
    // 2
    // UdpConnector收到一个未静音的音频帧（'F'开头），交给AudioSynthesizer放入对应队列
    connect(connector_, &UdpConnector::sig_oneMsgReady, &synthesizer_,
            &AudioSynthesizer::onOneFrameIn);
    // 3
    // UdpConnector收到一个静音的音频帧（'f'开头），交给AudioSynthesizer记录此用户还在线
    connect(connector_, &UdpConnector::sig_oneEmptyFrameReady, &synthesizer_,
            &AudioSynthesizer::onOneEmptyMsgIn);

    // 信号传递:
    // 输入音量变化
    connect(&collector_, &AudioCollector::sig_audioVolumeReady, this,
            &AudioChat::sig_collectorVolumeReady);
    // 用户音量变化
    connect(&synthesizer_, &AudioSynthesizer::sig_userVolumeReady, this,
            &AudioChat::sig_userVolumeReady);
    // 用户列表变化
    connect(&synthesizer_, &AudioSynthesizer::sig_userListReady, this,
            &AudioChat::sig_userListReady);
    // 用户静音状态变化
    connect(&synthesizer_, &AudioSynthesizer::sig_userIsMutedStatusReady, this,
            &AudioChat::sig_userIsMutedStatusReady);
}

void AudioChat::SetInputDevice(QAudioDeviceInfo info) {
    this->collector_.SetInputDevice(info);
}

void AudioChat::SetUserVolume(QString name, int volume /*[0,200]*/) {
    this->synthesizer_.SetVolume(name, volume);
}

void AudioChat::SetIsMuted(bool is_muted) {
    connector_->SetIsMuted(is_muted);
}

bool AudioChat::GetIsMuted() {
    return connector_->GetIsMuted();
}
