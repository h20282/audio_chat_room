#pragma once

#include <vector>

#include <QHostAddress>
#include <QMutex>
#include <QUdpSocket>

#include "audio_codec/Decoder.h"
#include "audio_codec/Encoder.h"

class UdpConnector : public QObject {
    Q_OBJECT

public:
    UdpConnector(QString user_name, int room_id);
    ~UdpConnector();
    void ToggleMute();
    bool IsMuted();
    void SetMuted(bool isMuted);

signals:
    void SigOneMsgReady(QString name, std::vector<char> data);
    void SigOneEmptyFrameReady(QString name);

public slots:
    void onAudioFrameReady(std::vector<char> frame);

private slots:
    void onUdpReadyRead();

private:
    QUdpSocket *udp_socket_ = nullptr;
    QHostAddress destaddr_;
    quint16 port_ = 0;
    QMutex mutex_;
    QString user_name_;
    int room_id_ = 0;
    bool is_muted_ = false;
    Encoder *encoder_;
    QMap<QString, Decoder *> decoders_;

    std::vector<char> recv_buff;
};
