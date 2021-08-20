#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "./net_api/pack_def.h"
#include "AbstractAudioFrameProvider.h"

#include <QThread>
#include <QQueue>
#include <QObject>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QMutex>
#include <QMutexLocker>
#include <QByteArray>

#include <QtNetwork/QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QDebug>

// 负责获取数据并播放音频，依赖于抽象类AbstractAudioFrameProvider，而不依赖于具体细节，耦合较低
class AudioPlayer : public QThread
{
    Q_OBJECT
public:
    AudioPlayer();
    ~AudioPlayer() override;
    void run() override;
    void setProvider(AbstractAudioFrameProvider *provider);

//public slots:
//    void onAudioFrameReady(AudioFrame frame);
private:

    QAudioOutput *m_output;
    QIODevice *m_audioIo;

    AbstractAudioFrameProvider *m_provider=nullptr;

//    QQueue<AudioFrame> m_queue;

    QMutex m_mutex;

    bool m_playing;
};




#endif // AUDIOPLAYER_H
