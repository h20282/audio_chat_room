#include "Encoder.h"


//g++ Untitled.cpp -o Untitled.exe -I D:/ffmpeg/ffmpeg-4.0.2-win64-dev/include  -D__STDC_CONSTANT_MACROS -L D:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib -lavcodec -lswresample -lavutil -lavformat -std=c++11; ./Untitled.exe
#include <iostream>

#define ERROR(msg) cout << "error in file :" << __FILE__\
                            << ", func: " << __func__ \
                            <<", line: " << __LINE__ \
                            << " (msg = '" << msg << "')" << endl;

#define cout qDebug()

using namespace std;

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>

}

#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swresample.lib")



char aac_adts_header[7];
ZipedFrame::ZipedFrame(int len, void* base){
    this->len = len+7;
    data = new unsigned char[this->len];
    memcpy(this->data, aac_adts_header, 7);
    memcpy(this->data+7, base, len);
}
ZipedFrame::~ZipedFrame(){
    delete data;
}


static AVCodec* codec;
static AVCodecContext* codecContext;
static AVFrame* avFrame;
static SwrContext* swrContext = NULL;

static int chanCfg = 2;            //MPEG-4 Audio Channel Configuration. 2 Channel front-center
static int init_aac_header() {
    memset(aac_adts_header, 0, sizeof(aac_adts_header));
    int profile = 2;   //AAC LC
    int freqIdx = 0x04;   //44100HZ

    aac_adts_header[0] = (char)0xFF;      // 11111111     = syncword
    aac_adts_header[1] = (char)0xF1;      // 1111 1 00 1  = syncword MPEG-2 Layer CRC
    aac_adts_header[2] = (char)(((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
    aac_adts_header[6] = (char)0xFC;

    return 0;
}
/**
 * @brief 向文件fp写入一个该pkt的ADTS HEADER， 如果fp==NULL则输出到控制台
 *
 * @param fp 文件指针
 * @param pkt 该packet
 * @return int
 */
static int write_aac_header(FILE* fp, const AVPacket* pkt) {
    aac_adts_header[3] = (char)(((chanCfg & 3) << 6) + ((7 + pkt->size) >> 11));
    aac_adts_header[4] = (char)(((7 + pkt->size) & 0x7FF) >> 3);
    aac_adts_header[5] = (char)((((7 + pkt->size) & 7) << 5) + 0x1F);

    if (fp==NULL){
//        auto p = reinterpret_cast<unsigned char*>(aac_adts_header);
//        for (int i = 0; i < 7; i++){
//            printf("%02x ", p[i]);
//        }
//        cout << endl;

        return 0;
    } else {
        return fwrite(aac_adts_header, 7, 1, fp);
    }
}



void initEncoder(){
    init_aac_header();
    av_register_all();
    avcodec_register_all();
    codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec){
        cout << "avcodec_find_encoder failed" << endl;
    }

    //配置编码器上下文
    codecContext = avcodec_alloc_context3(codec);

    if (!codecContext){
        cout << "avcodec_alloc_context3 failed" << endl;
    }

    codecContext->sample_rate = 44100;
    codecContext->channels = 2;
    codecContext->channel_layout = AV_CH_LAYOUT_STEREO; // 立体声
    codecContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    codecContext->bit_rate = 64000;
    codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    //打开音频的编码器
    int ret = avcodec_open2(codecContext, codec, NULL);
    if (ret < 0){
        cout << "avcodec_open2 failed" << endl;
    }

    cout << "codecContext->frame_size = " << codecContext->frame_size << endl;


    swrContext = swr_alloc_set_opts(
        swrContext, codecContext->channel_layout, codecContext->sample_fmt, 44100,//输出的音频参数
        AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, 44100,//输入的音频参数
        0, 0
    );

    if (!swrContext){
        cout << "swr_alloc_set_opts failed" << endl;
    }

    ret = swr_init(swrContext);
    if (ret < 0){
        cout << "swr_init failed" << endl;
    }

    avFrame = av_frame_alloc();
    avFrame->format = AV_SAMPLE_FMT_FLTP;
    avFrame->channels = 2;
    avFrame->channel_layout = AV_CH_LAYOUT_STEREO;
    // avFrame->nb_samples = 1024;//一帧音频的样本数量
    avFrame->nb_samples = 1024;//一帧音频的样本数量

    ret = av_frame_get_buffer(avFrame, 0);
    if (ret < 0) {
        cout << "av_frame_get_buffer failed" << endl;
    }

}


/**
 * @brief 从buff所指向的内存取4096个字节进行编码
 *
 * @param buff 指向4096长度的pcm数据
 * @return ZipedFrame* 编码失败返回NULL
 */
ZipedFrame* encodeFrame(void *buff){
    int len = swr_convert(swrContext, avFrame->data, avFrame->nb_samples, //重采样之后的数据
                    (const uint8_t**)&buff, avFrame->nb_samples  //重采样之前的数据
        );
    if (len <= 0){ return NULL; }
    AVPacket pkt;
    av_init_packet(&pkt);

    //将重采样的数据发送到编码线程
    int ret = avcodec_send_frame(codecContext, avFrame);
    if (ret != 0) { return NULL; }

    ret = avcodec_receive_packet(codecContext, &pkt);
    if (ret != 0){ return NULL; }

    write_aac_header(NULL, &pkt);
    return (new ZipedFrame(pkt.size, pkt.data));

}

void closeEncoder(){
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);
    cout << "ok!" << endl;
}

