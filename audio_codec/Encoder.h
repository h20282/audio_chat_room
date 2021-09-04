#pragma once

#include <vector>

#include <qdebug.h>
#include <QQueue>

#include "structs/AudioFrame.h"
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
};
