#pragma once

// 是否将从麦克风中采集到的声音存储到文件 "collected.pcm" 中
//#define SAVE_COLLECTED_PCM_INTO_FILE
#define COLLECTED_PCM_PA5TH "collected.pcm"

#include <QAudio>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QThread>

#include "spdlog/spdlog.h"

#include "../structs/AudioFrame.h"
#include "Config.h"

// 负责从麦克风中采集数据，压入队列中
class AudioCollector : public QThread {
    Q_OBJECT

public:
    AudioCollector();
    ~AudioCollector() override;
    void run() override;
    void SetInputDevice(QAudioDeviceInfo info);

signals:
    void sig_audioFrameReady(AudioFrame frame);
    void sig_audioVolumeReady(double volume);

private slots:
    void onReadyRead();

private:
    QAudioInput *input_;
    QIODevice *inputDevice_;
    QMutex mutex_;

#ifdef SAVE_COLLECTED_PCM_INTO_FILE
    FILE *m_fp;
#endif
};
