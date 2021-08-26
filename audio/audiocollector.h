#ifndef AUDIOCOLLECTOR_H
#define AUDIOCOLLECTOR_H


// 是否将从麦克风中采集到的声音存储到文件 "collected.pcm" 中
//#define SAVE_COLLECTED_PCM_INTO_FILE
#define COLLECTED_PCM_PATH "collected.pcm"

#include "Config.h"
#include "../structs/AudioFrame.h"

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

signals:
    void sig_audioFrameReady(AudioFrame frame);
    void sig_audioVolumeReady(double volume);

private slots:
    void onReadyRead();

private:
    QAudioInput *m_input;
    QIODevice *m_inputDevice;
    QMutex m_mutex;

#ifdef SAVE_COLLECTED_PCM_INTO_FILE
    FILE *m_fp; // todo: delete this
#endif
};

#endif // AUDIOCOLLECTOR_H
