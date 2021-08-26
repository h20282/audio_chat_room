#ifndef AUDIOSYNTHESIZER_H
#define AUDIOSYNTHESIZER_H

#include "../structs/AudioFrame.h"
#include "../structs/Msg.h"
#include "AbstractAudioFrameProvider.h"

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QMap>
#include <QQueue>
#include <QTimer>
#include <QString>
#include <QList>
#include <cmath>

class AudioSynthesizer : public QObject, public AbstractAudioFrameProvider
{
    Q_OBJECT
public:
    AudioSynthesizer();
    ~AudioSynthesizer();
    AudioFrame getAudioFrame() override;
    QList<QString> getUserList();
    void setVolume(QString name, int volume);

private:
    AudioFrame synthese();

signals:
    void sig_userVolumeReady(QString name, double volume);
    void sig_userListReady(QList<QString> list);
    void sig_userIsMutedStatusReady(QMap<QString, bool> userStatus); // 返回用户是否被静音了

public slots:
    void onOneFrameIn(Msg msg);
    void onOneEmptyMsgIn(QString userName);

private:
    QMap<QString, QQueue<AudioFrame>> m_queues;
    QMap<QString, bool> m_userIsMutedStatusReady;
    QMap<QString, time_t> m_lastOnlineTime;
    QMap<QString, int> m_volume;

    QMutex m_mutex;
    QTimer m_timer;

};

#endif // AUDIOSYNTHESIZER_H
