#ifndef AUDIOCOLLECTOR_H
#define AUDIOCOLLECTOR_H

#include "net_api/pack_def.h"
#include "net_api/udp_net.h"
#include "audioplayer.h"
#include "audiosynthesizer.h"

#include <QObject>
#include <QThread>

#include <QMutex>
#include <QMutexLocker>

#include <QDebug>

#include <QAudio>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QQueue>

#include <QtNetwork/QUdpSocket>
#include <QHostAddress>

// 负责从麦克风中采集数据，压入队列中
class AudioCollector : public QThread
{
    Q_OBJECT
public:
    AudioCollector();
    ~AudioCollector();
    void run() override;
    void setInputDevice(QAudioDeviceInfo info);

    void Init();

    void firstConnect();

    void setUdpRoomId(int room_id);


    // 加入房间，同一个房间内的人互相能听到声音（开启声音采集、传输、合成、播放）
    void joinRoom(int roomId);
    void setUserVolume(QString name, int volume /*[0,200]*/){
        this->m_synthesizer.setVolume(name, volume);
    }
    bool getIsMuted(){
        return m_connector->getIsMuted();
    }
    void changeMuteState(){
        m_connector->changeMuteState();
    }

    void MuteUser() {
        m_connector->MuteUser();
    }

    void UnMuteUser() {
        m_connector->UnMuteUser();
    }


signals:
    void sig_audioFrameReady(AudioFrame frame);
    void sig_audioVolumeReady(double volume);

    void sig_collectorVolumeReady(double volume);  // 输入设备音量，范围[0, 1)
    void sig_userVolumeReady(QString name, double volume);  // 各其他用户音量，范围[0, 1)
    void sig_userListReady(QList<QString> list); // 用户列表

public slots:
    void onReadyRead();

    void PauseAudio();
    void ResumeAudio(QAudioDeviceInfo info);

private:

    QAudioInput* createAudioInput(QAudioDeviceInfo info=QAudioDeviceInfo::defaultInputDevice());
    QAudioInput *m_input;
    QIODevice *m_inputDevice;
    QAudioFormat format;

    UdpNet       *m_connector;
    AudioPlayer         m_player;
    AudioSynthesizer    m_synthesizer;

    bool is_first_connect;

    QMutex m_mutex;

    int cnt = 0;

    enum ENUM_AUDIO_STATE{state_stop , state_play , state_pause };
    int m_playState;
//    FILE *fp; // todo: delete this
};

#endif // AUDIOCOLLECTOR_H
