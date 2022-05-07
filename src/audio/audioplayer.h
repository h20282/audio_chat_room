#pragma once

#include <QAudioFormat>
#include <QAudioOutput>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QThread>

#include "spdlog/spdlog.h"

#include "AbstractAudioFrameProvider.h"
#include "Config.h"

// 负责获取数据并播放音频，依赖于抽象类AbstractAudioFrameProvider，而不依赖于具体细节，耦合较低
// 使用：先 setProvider() 然后 start()
class AudioPlayer : public QThread {
    Q_OBJECT

public:
    AudioPlayer();
    ~AudioPlayer() override;
    void run() override;
    void SetProvider(AbstractAudioFrameProvider *provider);

private:
    QAudioOutput *output_;
    QIODevice *audio_io_;

    AbstractAudioFrameProvider *m_provider = nullptr;
    QMutex mutex_;
    bool playing_;
    std::map<int, int> cnt_;
};
