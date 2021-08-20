#ifndef AUDIOSYNTHESIZER_H
#define AUDIOSYNTHESIZER_H

#include "./net_api/pack_def.h"
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
    void sig_oneFrameReady(AudioFrame frame);
    void sig_userVolumeReady(QString name, double volume);
    void sig_userListReady(QList<QString> list);

public slots:
    void onOneFrameIn(Msg msg);
    void onOneEmptyMsgIn(QString userName);

private:
    QMap<QString, QQueue<AudioFrame>> m_queues;     //用户：音频队列
    QQueue<AudioFrame> m_output;                //输出的音频帧存在队列中
    QMap<QString, time_t> m_lastOnlineTime;     //上次在线事件
    QMap<QString, int> m_volume;            //每个用户的音量

    QMutex m_mutex;

};

#endif // AUDIOSYNTHESIZER_H
