#ifndef AUDIOCHAT_H
#define AUDIOCHAT_H

#include<QObject>
#include "./net_api/udp_net.h"
#include "audiocollector.h"
#include "audioplayer.h"
#include "audiosynthesizer.h"


class AudioChat: public QObject
{
    Q_OBJECT
public:
    AudioChat();
    ~AudioChat();

    // 加入房间，同一个房间内的人互相能听到声音（开启声音采集、传输、合成、播放）
    void joinRoom(int roomId);
    void setInputDevice(QAudioDeviceInfo info);
    void setUserVolume(QString name, int volume /*[0,200]*/){
        this->m_synthesizer.setVolume(name, volume);
    }
    bool getIsMuted(){
        return m_connector->getIsMuted();
    }
    void changeMuteState(){
        m_connector->changeMuteState();
    }

signals:
    void sig_collectorVolumeReady(double volume);  // 输入设备音量，范围[0, 1)
    void sig_userVolumeReady(QString name, double volume);  // 各其他用户音量，范围[0, 1)
    void sig_userListReady(QList<QString> list); // 用户列表

private:
    UdpNet       *m_connector;
    AudioCollector      m_collector;
    AudioPlayer         m_player;
    AudioSynthesizer    m_synthesizer;
};

#endif // AUDIOCHAT_H
