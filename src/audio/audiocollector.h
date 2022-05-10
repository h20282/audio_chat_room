#pragma once

#include <map>
#include <vector>

#include <QAudio>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

#include "spdlog/spdlog.h"

#include "Config.h"
#include "audio_codec/types.h"

// 负责从麦克风中采集数据，压入队列中
class AudioCollector : public QObject {
    Q_OBJECT

public:
    AudioCollector();
    ~AudioCollector() override;
    void Start();
    void SetInputDevice(QAudioDeviceInfo info);

signals:
    void SigAudioFrameReady(codec::AudioData frame);
    void SigAudioVolumeReady(double volume /*[0, 1]*/);

private slots:
    void onReadyRead();

private:
    QAudioInput *input_{};
    QIODevice *inputDevice_{};
    QMutex mutex_;
    std::map<int, int> cnt_;
};
