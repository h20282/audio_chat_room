#pragma once

#include <cassert>
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
#include "audio_codec/types.h"

namespace audio {
class Data {
public:
    explicit Data(codec::AudioData data) : data_(data), curr_(0) {}
    ~Data() = default;

public:
    std::size_t GetSize() const {
        return data_->size() - curr_;
    }
    uint8_t *GetBase() const {
        return data_->data() + curr_;
    }
    void Pop(std::size_t size) {
        curr_ += size;
        assert(curr_ <= data_->size() && "audio::Data::curr_ out of range");
    }

private:
    codec::AudioData data_;
    std::size_t curr_;
};
}  // namespace audio

class AudioSynthesizer : public QObject, public AbstractAudioFrameProvider {
    Q_OBJECT
public:
    AudioSynthesizer();
    ~AudioSynthesizer() override;
    std::vector<char> GetAudioFrame() override;
    QList<QString> GetUserList();
    void SetVolume(QString name, int volume /*[0, 200]*/);

private:
    std::vector<char> Synthese();

signals:
    void SigUserVolumeReady(QString name, double volume /*[0, 1]*/);
    void SigUserListReady(QList<QString> list);
    void SigUserIsMutedStatusReady(QMap<QString, bool> userStatus);

public slots:
    void onOneFrameIn(QString name, codec::AudioData pcm_data);
    void onOneEmptyMsgIn(QString userName);

private:
    QMap<QString, QQueue<audio::Data>> queues_;
    QMap<QString, bool> is_muted_;
    QMap<QString, time_t> last_online_t_;
    QMap<QString, int> volume_;

    QMutex mutex_;
    QTimer timer_;
    std::map<int, int> cnt_;
};
