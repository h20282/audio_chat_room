#pragma once

#include <qdebug.h>
#include <utility>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <bitset>
#include <iostream>

extern "C" {
#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libavutil\frame.h>
#include <libswresample\swresample.h>
}

#include "Config.h"


#define cout qDebug()

class Decoder {

public:
    Decoder();
    ~Decoder();
    /*
     * @buff：指向aac帧
     * @len：aac帧长度（包括帧头、数据）
     *
     * @return 解码后的数据（用完需delete）、数据长度
     */
    std::pair<unsigned char *, int> DecodeFrame(void *buff, int len);

private:
    void CloseDecoder();
    void InitDecoder();
    AVPacket *packet_ = nullptr;
    AVCodecContext *cod_ctx_ = nullptr;
    AVCodec *cod_ = nullptr;
    SwrContext *convert_ctx_ = nullptr;
    AVFrame *frame_ = nullptr;
    uint8_t *buffer_ = nullptr;
    int buffer_size_ = 0;
};
