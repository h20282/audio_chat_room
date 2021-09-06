#pragma once

#include <cmath>

#include <vector>

#include <QList>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QQueue>
#include <QString>
#include <QThread>
#include <QTimer>

#include "AbstractAudioFrameProvider.h"

class AudioSynthesizer : public QObject, public AbstractAudioFrameProvider {
    Q_OBJECT
public:
    AudioSynthesizer();
    ~AudioSynthesizer() override;
    std::vector<char> GetAudioFrame() override;
    QList<QString> GetUserList();
    void SetVolume(QString name, int volume/*[0, 200]*/);

private:
    std::vector<char> Synthese();

signals:
    void SigUserVolumeReady(QString name, double volume/*[0, 1]*/);
    void SigUserListReady(QList<QString> list);
    void SigUserIsMutedStatusReady(QMap<QString, bool> userStatus);

public slots:
    void onOneFrameIn(QString name, std::vector<char> pcm_data);
    void onOneEmptyMsgIn(QString userName);

private:
    QMap<QString, QQueue<std::vector<char>>> queues_;
    QMap<QString, bool> is_muted_;
    QMap<QString, time_t> last_online_t_;
    QMap<QString, int> volume_;

    QMutex mutex_;
    QTimer timer_;
};
