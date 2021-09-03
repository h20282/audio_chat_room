#ifndef UDPCONNECTOR_H
#define UDPCONNECTOR_H

// 收发时是否加入编解码模块
#define CODEC
// 是否输出编码器编码前后数据到文件 "udp_net.pcm" 和 "udp_net.aac" 中
//#define SAVE_ENCODE_IO_INTO_FILE

#include <vector>

#include <QHostAddress>
#include <QMutex>
#include <QUdpSocket>

#include "../audio_codec/Decoder.h"
#include "../audio_codec/Encoder.h"
#include "../structs/Msg.h"

class UdpConnector : public QObject {
    Q_OBJECT

public:
    UdpConnector(QString user_name, int room_id);
    ~UdpConnector();
    void ChangeMuteState();
    bool GetIsMuted();
    void SetIsMuted(bool isMuted);

signals:
    void sig_oneMsgReady(Msg msg);
    void sig_oneEmptyFrameReady(QString name);

public slots:
    void onAudioFrameReady(AudioFrame frame);

private slots:
    void onUdpReadyRead();

private:
    QUdpSocket *udp_socket_;
    QHostAddress destaddr_;
    quint16 port_;

    QMutex mutex_;

    QString user_name_;
    int room_id_;
    bool is_muted_ = false;

    Encoder *encoder_;
    QMap<QString, Decoder *> decoders_;

#ifdef SAVE_ENCODE_IO_INTO_FILE
    FILE *fp_aac;  // todo: delete it!
    FILE *fp_pcm;  // todo: delete it!
    QMap<QString, FILE *> m_files;
    QMap<QString, FILE *> m_filesPCM;
#endif
};

#endif  // UDPCONNECTOR_H
