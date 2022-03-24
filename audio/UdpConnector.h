#pragma once

#include <map>
#include <vector>

#include <QHostAddress>
#include <QMutex>
#include <QUdpSocket>

#include "audio_codec/opus_decoder.h"
#include "audio_codec/opus_encoder.h"

class UdpConnector : public QObject {
    Q_OBJECT

public:
    UdpConnector(QString user_name, int room_id);
    ~UdpConnector();
    void ToggleMute();
    bool IsMuted();
    void SetMuted(bool isMuted);

signals:
    void SigOneMsgReady(QString name, AudioData);
    void SigOneEmptyFrameReady(QString name);

public slots:
    void onAudioFrameReady(AudioData frame);

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
    OEncoder encoder_;
    std::map<QString, ODecoder> decoders_;

    std::vector<char> recv_buff;
};
