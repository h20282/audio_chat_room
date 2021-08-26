#ifndef UDPCONNECTOR_H
#define UDPCONNECTOR_H

// 收发时是否加入编解码模块
#define CODEC
// 是否输出编码器编码前后数据到文件 "udp_net.pcm" 和 "udp_net.aac" 中
//#define SAVE_ENCODE_IO_INTO_FILE

#include "../structs/Msg.h"
#include "../audio_codec/Encoder.h"
#include "../audio_codec/Decoder.h"

#include <QUdpSocket>
#include <QHostAddress>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>


class UdpConnector: public QObject
{
    Q_OBJECT

public:
    UdpConnector(QString userName, int roomId);
    ~UdpConnector();
    void changeMuteState();
    bool getIsMuted();
    void setIsMuted(bool isMuted);

signals:
    void sig_oneMsgReady(Msg msg);
    void sig_oneEmptyFrameReady(QString name);

public slots:
    void onAudioFrameReady(AudioFrame frame);

private  slots:
    void onUdpReadyRead();

private:
    QUdpSocket *m_udpSocket;
    QHostAddress m_destaddr;
    quint16 m_port;

    QMutex m_mutex;

    QString m_userName;
    int m_roomId;
    bool m_isMuted = false;

    Encoder *m_encoder;
    QMap<QString, Decoder*> m_decoders;

#ifdef SAVE_ENCODE_IO_INTO_FILE
    FILE *fp_aac; // todo: delete it!
    FILE *fp_pcm; // todo: delete it!
    QMap<QString, FILE*> m_files;
    QMap<QString, FILE*> m_filesPCM;
#endif
};

#endif // UDPCONNECTOR_H
