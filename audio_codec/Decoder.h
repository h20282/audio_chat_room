#ifndef DECODER_H
#define DECODER_H

#include <qdebug.h>
#include <utility>


void initDecoder();

/*
 * @buff：指向aac帧
 * @len：aac帧长度（包括帧头、数据）
 *
 * @return 解码后的数据（用完需delete）、数据长度
 */
std::pair<unsigned char*, int> decodeFrame(void*buff, int len);
void closeDecoder();


#endif // DECODER_H
