#include "AudioSynthesizer.h"

AudioSynthesizer::AudioSynthesizer() {

    QObject::connect(&m_timer, &QTimer::timeout, [this](){
        emit sig_userListReady(getUserList());
        emit sig_userIsMutedStatusReady(m_userIsMutedStatusReady);
    });
    m_timer.start(300);
}

AudioSynthesizer::~AudioSynthesizer() {

}

AudioFrame AudioSynthesizer::getAudioFrame() {
    QMutexLocker locker(&m_mutex);
    return this->synthese();
}

double f(int x) {
    return (pow(1.0223, x)-1) * (100/8.07478);
}

// 从各个队列中获取数据，合成为一个音频帧
AudioFrame AudioSynthesizer::synthese() {
    AudioFrame frame;
    memset(&frame, 0, sizeof(0));
    int maxFrameLen = 0;
    double n = 0; // 权值累和
    static int v[AUDIO_FRAME_LEN/2];
    memset(v, 0, sizeof(v));
    if (m_queues.size())
    for ( auto iter=m_queues.begin(); iter!=m_queues.end(); iter++ ) {
        const auto &name = iter.key();
        auto &queue = iter.value();
        if ( m_volume.find(name)==m_volume.end() ) {
            n = m_volume[name] = 100;
        }

        if (queue.size()) {
            auto x = f(m_volume[name]); // 权值
            n += 100;
            auto currFrame = queue.dequeue();
            if (currFrame.len != AUDIO_FRAME_LEN) {
//                qDebug() << "not AUDIO_FRAME_LEN:" << currFrame.len;
            }
            if (currFrame.len>sizeof(AudioFrame)-4) {
                continue;
            }

            maxFrameLen = qMax(maxFrameLen, currFrame.len);

            auto base_b = reinterpret_cast<short*>(&currFrame.buff[0]);
            for (int i=0; i<maxFrameLen/2; i++) {
                v[i] += (int)base_b[i]*x;
            }

        }
    }
    if (n!=0) {

        auto base_a = reinterpret_cast<short*>(&frame.buff[0]);
        for (int i=0; i<maxFrameLen/2; i++) {
            if (abs(v[i]/n)>32767) {
                base_a[i] = 32767 * (v[i]/abs(v[i]));
            } else {
                base_a[i] = v[i]/n;
            }
        }
        frame.len = maxFrameLen;
    }
    qDebug() << "-->n = " << n << "maxFrameLen =" << maxFrameLen;
    return frame;
}

// 获取在线用户列表，超过3s没信号的则会被忽略
QList<QString> AudioSynthesizer::getUserList(){
    QList<QString> ret;
//    QMutexLocker locker(&m_mutex);
    qDebug() << "m_lastOnlineTime.size()" << m_lastOnlineTime.size();
    for ( auto iter=m_lastOnlineTime.begin(); iter!=m_lastOnlineTime.end(); iter++ ) {
        QString name = iter.key();
        auto lastTime = iter.value();
        if (time(nullptr)-lastTime < 2) {
            qDebug() << name;
            ret.push_back(name);
        }
    }
    return ret;
}

void AudioSynthesizer::setVolume(QString name, int volume) {
    if (volume>=0 && volume<=200) {
        m_volume[name] = volume;
    }
}

// 每当有一个消息来临时，记录“该用户此时有信号”、入队
void AudioSynthesizer::onOneFrameIn(Msg msg) {
//    qDebug() << "one msg from " << msg.name << " frame len = " << msg.frame.len;
    QMutexLocker locker(&m_mutex);
    QString name(msg.name);
    m_userIsMutedStatusReady[name] = false;
//#define DUI_QI
#ifdef DUI_QI
    // 用于重组成完整的pcm音频帧（长度固定为AUDIO_FRAME_LEN）
    // todo:将此变量放到成员变量中，否则多个AudioSynthesizer对象将共用此变量
    static QMap<QString, QQueue<AudioFrame>> s_queues;
    static int s_currIdx = 0;
    s_queues[name].enqueue(msg.frame);

    auto getRestBytesFromCurrQueue = [&](){
        int cnt = 0;
        for ( auto i : s_queues[name] ) {
            qDebug() << "i.len " << i.len;
            cnt += i.len;
        }
        return cnt-s_currIdx;
    };
//    int bytesCnt = 0;
//    for ( auto i : s_queues[name] ) {
//        bytesCnt += i.len;
//    }

    while ( getRestBytesFromCurrQueue() >= AUDIO_FRAME_LEN ) {
        qDebug() << "getRestBytesFromCurrQueue()" << getRestBytesFromCurrQueue() << "s_queues[name].size()" << s_queues[name].size();
        int bytedNeed = AUDIO_FRAME_LEN;
        int buffIdx = 0;
        AudioFrame frame;
        frame.len = AUDIO_FRAME_LEN;

        while ( true ) {

            qDebug() << "befor call front(), s_queues[name]" << s_queues[name].size();
            int frontRestBytes = s_queues[name].front().len-s_currIdx;
            if ( frontRestBytes < bytedNeed ) {
                memcpy(frame.buff+buffIdx, s_queues[name].front().buff+s_currIdx, frontRestBytes); // 这里可能存在bug导致段错误
                buffIdx += frontRestBytes;
                bytedNeed -= frontRestBytes;
                s_queues[name].dequeue();
                s_currIdx = 0;
            } else {
                memcpy(frame.buff+buffIdx, s_queues[name].front().buff+s_currIdx, bytedNeed);
                s_currIdx += bytedNeed;
                qDebug() << __FILE__ << "break";
                break;
            }
        }
        m_queues[name].enqueue(frame);
    }
#endif

#ifndef DUI_QI
//    qDebug() << "msg.frame.len" << msg.frame.len;
    m_queues[name].enqueue(msg.frame);
#endif

    m_lastOnlineTime[name] = time(0);
    if (m_queues[name].size()>10) {
        qDebug() << "droped a audio frame";
        m_queues[name].dequeue();
    }
    if (m_volume.find(name)!=m_volume.end()){
        emit sig_userVolumeReady(name, msg.frame.getMaxVolume() * m_volume[name]/200);
    }

}

// 一个'f'开头的静音消息
void AudioSynthesizer::onOneEmptyMsgIn(QString userName) {
    QMutexLocker locker(&m_mutex);
    m_userIsMutedStatusReady[userName] = true;
    m_lastOnlineTime[userName] = time(0);
}
