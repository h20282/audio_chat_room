#include "audiosynthesizer.h"

AudioSynthesizer::AudioSynthesizer()
{
}

AudioSynthesizer::~AudioSynthesizer()
{
    this->requestInterruption();
    QThread::msleep(100);
}

////每隔40ms合成一次，并发送信号
void AudioSynthesizer::run()
{
    //    while (!this->isInterruptionRequested()) {
    //        QThread::sleep(2);

    //    }

    //    qDebug() << "void AudioSynthesizer::run()";
    //    int cnt = 0;
    //    while (!this->isInterruptionRequested()) {
    //        QThread::msleep(20);
    //        qDebug() << "ehxsafa";
    ////        QMutexLocker locker(&m_mutex);
    ////        while(m_output.size()<5){
    ////            qDebug() << "ehxsafdsafdsa";
    ////            m_output.enqueue(this->synthese());
    ////        }
    ////        emit sig_userListReady(this->getUserList());
    //    }
}

AudioFrame AudioSynthesizer::getAudioFrame()
{
    QMutexLocker locker(&m_mutex);
    return this->synthese();
    if (m_output.size())
    {
        qDebug() << "has";
        return this->synthese();
        //        return m_output.dequeue();
    }
    else
    {
        qDebug() << "no has";
        AudioFrame frame;
        frame.len = 0;
        return frame;
    }
}

// 从各个队列中获取数据，合成为一个音频帧
AudioFrame AudioSynthesizer::synthese()
{
    AudioFrame frame;
    memset(&frame, 0, sizeof(0));
    int maxFrameLen = 0;
    int n = 0; // 权值累和
    static int v[AUDIO_FRAME_LEN / 2];
    memset(v, 0, sizeof(v));
    for (auto iter = m_queues.begin(); iter != m_queues.end(); iter++)
    {
        const auto &name = iter.key();
        auto &queue = iter.value();
        if (m_volume.find(name) == m_volume.end())
        {
            n = m_volume[name] = 100;
        }

        if (queue.size())
        {
            auto x = m_volume[name]; // 权值
            n += 100;
            auto currFrame = queue.dequeue();
            if (currFrame.len > sizeof(AudioFrame) - 4)
            {
                continue;
            }

            maxFrameLen = qMax(maxFrameLen, currFrame.len);

            auto base_b = reinterpret_cast<short *>(&currFrame.buff[0]);
            for (int i = 0; i < currFrame.len / 2; i++)
            {
                v[i] += (int)base_b[i] * x;
            }
        }
    }
    if (n != 0)
    {

        auto base_a = reinterpret_cast<short *>(&frame.buff[0]);
        for (int i = 0; i < maxFrameLen / 2; i++)
        {
            if (abs(v[i] / n) > 32767)
            {
                base_a[i] = 32767 * (v[i] / abs(v[i]));
            }
            else
            {
                base_a[i] = v[i] / n;
            }
        }
        frame.len = maxFrameLen;
    }
    //qDebug() << "-->n = " << n << "maxFrameLen =" << maxFrameLen;
    return frame;
}

// 获取在线用户列表，超过3s没信号的则会被忽略
QList<QString> AudioSynthesizer::getUserList()
{
    QList<QString> ret;
    //    QMutexLocker locker(&m_mutex);
    for (auto iter = m_queues.begin(); iter != m_queues.end(); iter++)
    {
        QString name = iter.key();
        if (time(nullptr) - m_lastOnlineTime[name] < 3)
        {
            ret.push_back(name);
        }
    }
    return ret;
}

void AudioSynthesizer::setVolume(QString name, int volume)
{
    if (volume >= 0 and volume <= 200)
    {
        m_volume[name] = volume;
    }
}

// 从服务器接收每一个消息来临时，记录“该用户此时有信号”、入队。
void AudioSynthesizer::onOneFrameIn(Msg msg)
{
    //qDebug() << "one msg from " << msg.name << " frame len = " << msg.frame.len;
    QMutexLocker locker(&m_mutex);
    QString name(msg.name);
    m_lastOnlineTime[name] = time(0);
    m_queues[name].enqueue(msg.frame);
    //队列长度为10，
    if (m_queues[name].size() > 10)
    {
        //qDebug() << "droped a audio frame";
        m_queues[name].dequeue();
    }
    //更新用户列表的时候会setVolume，因此会将用户存在m_volume内。
    if (m_volume.find(name) != m_volume.end())
        emit sig_userVolumeReady(name, msg.frame.getMaxVolume() * m_volume[name] / 200);
    static int i = 0;
    //收到10帧信息，发送过去更新用户列表界面的声音和图像画面
    if (++i % 20 == 0)
    {
        emit sig_userListReady(this->getUserList());
    }
}

// 一个'f'开头的静音消息
void AudioSynthesizer::onOneEmptyMsgIn(QString userName)
{
    QMutexLocker locker(&m_mutex);
    m_lastOnlineTime[userName] = time(0);
}
