#pragma once

#include <vector>

#include <QQueue>

#include "Config.h"

/*
 * 编码器，线程不安全，不支持多实例
 */
class Encoder {

public:
    Encoder();
    ~Encoder();
    void PushAudioFrame(std::vector<char> frame);
    std::vector<char> GetZipedFrame();

private:
    QQueue<std::vector<char>> queue_;
    std::vector<char> curr_frame_;
    int curr_idx_;
};
