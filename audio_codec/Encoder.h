#ifndef ENCODER_H
#define ENCODER_H

//是否将重分割前后的pcm输出到 "fp_before_resplit.pcm" 和 "fp_after_resplit.pcm"
//#define SAVE_RESPLIT_IO_INTO_FILE

#include "../structs/AudioFrame.h"
#include "Config.h"

#include <qdebug.h>
#include <QQueue>

struct ZipedFrame
{
    int len; // data所指内存字节数
    unsigned char* data;

    ZipedFrame(int len, void* base);
    ~ZipedFrame();
};


void initEncoder();
ZipedFrame* encodeFrame(void *buff); // 使用完记得delete
void closeEncoder();


/*
 * 编码器，线程不安全，不支持多实例
 * todo: 改为单例模式
 * 使用方法，getInstance获取实例（初始化编码器），关闭时delete获取到的实例
 */
class Encoder{


public:
    Encoder();

    void pushAudioFrame(AudioFrame frame);

    ZipedFrame *getZipedFrame();
private:

    QQueue<AudioFrame> m_queue;
    AudioFrame currFrame;
    int m_currIdx;

#ifdef SAVE_RESPLIT_IO_INTO_FILE
    FILE *fp_before_resplit;
    FILE *fp_after_resplit;
#endif
};





#endif // ENCODER_H
