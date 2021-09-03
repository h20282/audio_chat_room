#pragma once

//是否将重分割前后的pcm输出到 "fp_before_resplit.pcm" 和 "fp_after_resplit.pcm"
//#define SAVE_RESPLIT_IO_INTO_FILE

#include <qdebug.h>
#include <QQueue>

#include "../structs/AudioFrame.h"
#include "Config.h"

struct ZipedFrame {
    int len;  // data所指内存字节数
    unsigned char *data;

    ZipedFrame(int len, void *base);
    ~ZipedFrame();
};

/*
 * 编码器，线程不安全，不支持多实例
 * todo: 改为单例模式
 * 使用方法，getInstance获取实例（初始化编码器），关闭时delete获取到的实例
 */
class Encoder {

public:
    Encoder();
    ~Encoder();
    void PushAudioFrame(AudioFrame frame);

    // 用完delete
    ZipedFrame *GetZipedFrame();

private:
    QQueue<AudioFrame> queue_;
    AudioFrame curr_frame_;
    int curr_idx_;

#ifdef SAVE_RESPLIT_IO_INTO_FILE
    FILE *fp_before_resplit;
    FILE *fp_after_resplit;
#endif
};
