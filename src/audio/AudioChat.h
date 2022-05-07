#pragma once

#include <QObject>
#include <set>

#include "UdpConnector.h"
#include "audiocollector.h"
#include "audioplayer.h"
#include "audiosynthesizer.h"

class AudioChat : public QObject {
    Q_OBJECT

public:
    AudioChat();
    ~AudioChat();
    // 加入房间，同一个房间内的人互相能听到声音（开启声音采集、传输、合成、播放）
    void JoinRoom(QString user_name, int room_id);
    void SetInputDevice(QAudioDeviceInfo info);
    std::set<std::string> GetInputDevices();
    void SetUserVolume(QString name, int volume /*[0,200]*/);
    void SetMuted(bool is_muted);
    bool IsMuted();

signals:
    // 输入设备音量，范围[0, 1)
    void SigCollectorVolumeReady(double volume);
    // 各其他用户音量，范围[0, 1)
    void SigUserVolumeReady(QString name, double volume);
    // 用户列表
    void SigUserListReady(QList<QString> list);
    // 返回用户是否被静音了
    void SigUserIsMutedStatusReady(QMap<QString, bool> user_status);

private:
    AudioCollector collector_;
    AudioPlayer player_;
    AudioSynthesizer synthesizer_;
    UdpConnector *connector_;
};
