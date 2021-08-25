#ifndef AUDIOSYNTHESIZER_H
#define AUDIOSYNTHESIZER_H

#include "../structs/AudioFrame.h"
#include "../structs/Msg.h"
#include "AbstractAudioFrameProvider.h"

#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QMap>
#include <QQueue>
#include <QString>
#include <QList>

class AudioSynthesizer: public QThread, public AbstractAudioFrameProvider
{
    Q_OBJECT
public:
    AudioSynthesizer();
    ~AudioSynthesizer();
    void run() override;
    AudioFrame getAudioFrame() override;
    QList<QString> getUserList();
    void setVolume(QString name, int volume);

private:
    AudioFrame synthese();

signals:
    void sig_userVolumeReady(QString name, double volume);
    void sig_userListReady(QList<QString> list);

public slots:
    void onOneFrameIn(Msg msg);
    void onOneEmptyMsgIn(QString userName);

private:
    QMap<QString, QQueue<AudioFrame>> m_queues;
//    QQueue<AudioFrame> m_output;
    QMap<QString, time_t> m_lastOnlineTime;
    QMap<QString, int> m_volume;

    QMutex m_mutex;

};

#endif // AUDIOSYNTHESIZER_H
