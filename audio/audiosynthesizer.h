#pragma once

#include <cmath>

#include <QList>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QThread>
#include <QTimer>

#include "structs/AudioFrame.h"
#include "structs/Msg.h"
#include "AbstractAudioFrameProvider.h"

class AudioSynthesizer : public QObject, public AbstractAudioFrameProvider {
    Q_OBJECT
public:
    AudioSynthesizer();
    ~AudioSynthesizer() override;
    AudioFrame GetAudioFrame() override;
    QList<QString> GetUserList();
    void SetVolume(QString name, int volume);

private:
    AudioFrame Synthese();

signals:
    void SigUserVolumeReady(QString name, double volume);
    void SigUserListReady(QList<QString> list);
    void SigUserIsMutedStatusReady(QMap<QString, bool> userStatus);

public slots:
    void onOneFrameIn(Msg msg);
    void onOneEmptyMsgIn(QString userName);

private:
    QMap<QString, QQueue<AudioFrame>> queues_;
    QMap<QString, bool> is_muted_;
    QMap<QString, time_t> last_online_t_;
    QMap<QString, int> volume_;

    QMutex mutex_;
    QTimer timer_;
};
