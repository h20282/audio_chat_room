#ifndef DECODER_H
#define DECODER_H

#include <qdebug.h>
#include <utility>
#include "Config.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>
extern "C"{
#include <libavcodec\avcodec.h>
#include <libavutil\frame.h>
#include <libavformat\avformat.h>
#include <libswresample\swresample.h>
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")

#define cout qDebug()


void initDecoder();

/*
 * @buff：指向aac帧
 * @len：aac帧长度（包括帧头、数据）
 *
 * @return 解码后的数据（用完需delete）、数据长度
 */
std::pair<unsigned char*, int> decodeFrame(void*buff, int len);
void closeDecoder();





class Decoder{
public:
    Decoder();
    ~Decoder();
    std::pair<unsigned char*, int> decodeFrame(void*buff, int len);
private:
    void closeDecoder();
    void initDecoder();
    AVPacket 		*packet = NULL;
    AVCodecContext  *cod_ctx = NULL;
    AVCodec         *cod = NULL;
    SwrContext 		*convert_ctx = NULL;
    AVFrame 		*frame = NULL;
    uint8_t 		*buffer = NULL;
    int 			buffer_size = 0;
};


#endif // DECODER_H
