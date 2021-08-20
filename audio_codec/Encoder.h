#ifndef ENCODER_H
#define ENCODER_H

#include <qdebug.h>
#include <QQueue>
#include "../net_api/pack_def.h"

struct ZipedFrame
{
    int len; // data所指内存字节数
    unsigned char* data;

    ZipedFrame(int len, void* base);
    ~ZipedFrame();
};


void initEncoder();
ZipedFrame* encodeFrame(void *buff);
void closeEncoder();


/*
 * 编码器，单例模式，线程不安全
 * 使用方法，getInstance获取实例（初始化编码器），关闭时delete获取到的实例
 */
class Encoder{


public:
    Encoder(){
        initEncoder();
        this->m_currIdx = 0;
        qDebug() << "Encoder::Encoder() finished";
    }

    void pushAudioFrame(AudioFrame frame){
        m_queue.enqueue(frame);
    }

    ZipedFrame *getZipedFrame(){

        int restBytes = AUDIO_FRAME_LEN-m_currIdx + (m_queue.size()-1)*AUDIO_FRAME_LEN;

        if (restBytes > 4096) { // 队列里面有4096字节的数据
            /*      front                                                       tail
             * [--------------] <- [--------------] <- [--------------] <- [--------------]
             *           ^
             *           |
             *       m_currIdx
             */
            static char buff[4096];
            int restBytesInCurrFrame = AUDIO_FRAME_LEN - m_currIdx;
            if ( restBytesInCurrFrame > 4096 ){
                memcpy(buff, m_queue.front().buff+m_currIdx , 4096);
                m_currIdx += 4096;
            } else {
                memcpy(buff, m_queue.front().buff+m_currIdx, restBytesInCurrFrame);
                m_queue.dequeue();
                memcpy(buff + restBytesInCurrFrame, m_queue.front().buff, 4096-restBytesInCurrFrame);
                m_currIdx = restBytesInCurrFrame;
            }
            return encodeFrame(buff);

        } else {
            return nullptr;
        }

    }
private:

    QQueue<AudioFrame> m_queue;
    AudioFrame currFrame;
    int m_currIdx;
};







#endif // ENCODER_H
