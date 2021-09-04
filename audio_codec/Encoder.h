#pragma once

//是否将重分割前后的pcm输出到 "fp_before_resplit.pcm" 和 "fp_after_resplit.pcm"
//#define SAVE_RESPLIT_IO_INTO_FILE

#include <vector>

#include <qdebug.h>
#include <QQueue>

#include "../structs/AudioFrame.h"
#include "Config.h"


/*
 * 编码器，线程不安全，不支持多实例
 */
class Encoder {

public:
    Encoder();
    ~Encoder();
    void PushAudioFrame(AudioFrame frame);
    std::vector<char> GetZipedFrame();

private:
    QQueue<AudioFrame> queue_;
    AudioFrame curr_frame_;
    int curr_idx_;

#ifdef SAVE_RESPLIT_IO_INTO_FILE
    FILE *fp_before_resplit;
    FILE *fp_after_resplit;
#endif
};
