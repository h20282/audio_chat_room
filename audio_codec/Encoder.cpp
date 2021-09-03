#include "Encoder.h"

#include "log/log.h"

void initEncoder();
ZipedFrame *encodeFrame(void *buff);  // 使用完记得delete
void closeEncoder();

//#include <iostream>

using namespace std;

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
}

namespace {
char aac_adts_header[7];
}

ZipedFrame::ZipedFrame(int len, void *base) {
    this->len = len + 7;
    data = new unsigned char[static_cast<size_t>(this->len)];
    memcpy(this->data, aac_adts_header, 7);
    memcpy(this->data + 7, base, static_cast<size_t>(len));
}
ZipedFrame::~ZipedFrame() {
    delete data;
}

///////////////////////////////////////begin of class Encoder definition
Encoder::Encoder() {
    initEncoder();
    this->curr_idx_ = 0;
    qDebug() << "Encoder::Encoder() finished";

#ifdef SAVE_RESPLIT_IO_INTO_FILE
    this->fp_before_resplit = fopen("fp_before_resplit.pcm", "wb");
    this->fp_after_resplit = fopen("fp_after_resplit.pcm", "wb");
#endif
}

Encoder::~Encoder() {
    closeEncoder();
}

void Encoder::PushAudioFrame(AudioFrame frame) {
    queue_.enqueue(frame);
}

ZipedFrame *Encoder::GetZipedFrame() {

    int rest_bytes = 0;
    for (auto iter = queue_.begin(); iter != queue_.end(); iter++) {
        //        qDebug() << "iter.len" << (*iter).len;
        rest_bytes += (*iter).len;
    }
    rest_bytes -= curr_idx_;

    //        int rest_bytes = AUDIO_FRAME_LEN-m_currIdx +
    //        (m_queue.size()-1)*AUDIO_FRAME_LEN;

    if (rest_bytes > 4096) {  // 队列里面有4096字节的数据
        /*      front                                    tail
         * [--------------] <- [--------------] <- [--------------]
         *           ^
         *           |
         *       m_currIdx
         */
        static char buff[4096];  // warning:
                                 // 多对象多进程下将竞争使用此static变量导致错误

        int bytedNeed = sizeof(buff);
        int buffIdx = 0;

        while (true) {

            int front_rest_bytes = queue_.front().len - curr_idx_;
            if (front_rest_bytes < bytedNeed) {
                memcpy(buff + buffIdx, queue_.front().buff + curr_idx_,
                       static_cast<size_t>(front_rest_bytes));
                buffIdx += front_rest_bytes;
                bytedNeed -= front_rest_bytes;
                queue_.dequeue();
                curr_idx_ = 0;
            } else {
                memcpy(buff + buffIdx, queue_.front().buff + curr_idx_,
                       static_cast<size_t>(bytedNeed));
                curr_idx_ += bytedNeed;
                break;
            }
        }
        return encodeFrame(buff);
    } else {
        return nullptr;
    }
}
///////////////////////////////////////end of class Encoder definition

namespace {
AVCodec *codec;
AVCodecContext *codecContext;
AVFrame *avFrame;
SwrContext *swrContext = nullptr;

int chanCfg = 2;  // MPEG-4 Audio Channel Configuration. 2 Channel front-center
int init_aac_header() {
    memset(aac_adts_header, 0, sizeof(aac_adts_header));
    int profile = 2;  // AAC LC
    // AUDIO_SAM_RATE HZ
    int freqIdx;
    static int rates[] = {96000, 88000, 64000, 48000, 44100, 32000, 24000,
                          22000, 16000, 12000, 11025, 8000,  7350};
    for (int i = 0; i < sizeof(rates) / sizeof(rates[0]); i++) {
        if (rates[i] == AUDIO_SAM_RATE) {
            LOG_INFO("{}->{}", AUDIO_SAM_RATE, rates[i]);
            freqIdx = i;
        }
    }
    // 11111111     = syncword
    aac_adts_header[0] = static_cast<char>(0xff);
    // 1111 1 00 1  = syncword MPEG-2 Layer CRC
    aac_adts_header[1] = static_cast<char>(0xF1);
    aac_adts_header[2] = static_cast<char>(((profile - 1) << 6) +
                                           (freqIdx << 2) + (chanCfg >> 2));
    aac_adts_header[6] = static_cast<char>(0xFC);

    return 0;
}
}  // namespace
/**
 * @brief 向文件fp写入一个该pkt的ADTS HEADER， 如果fp==nullptr则输出到控制台
 *
 * @param fp 文件指针
 * @param pkt 该packet
 * @return int
 */
namespace {
void write_aac_header(const AVPacket *pkt) {
    aac_adts_header[3] =
            static_cast<char>(((chanCfg & 3) << 6) + ((7 + pkt->size) >> 11));
    aac_adts_header[4] = static_cast<char>(((7 + pkt->size) & 0x7FF) >> 3);
    aac_adts_header[5] = static_cast<char>((((7 + pkt->size) & 7) << 5) + 0x1F);
}
}  // namespace

void initEncoder() {
    init_aac_header();
    //    av_register_all();
    //    avcodec_register_all();
    codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec) { LOG_ERROR("avcodec_find_encoder failed"); }

    //配置编码器上下文
    codecContext = avcodec_alloc_context3(codec);

    if (!codecContext) { LOG_ERROR("avcodec_alloc_context3 failed"); }

    codecContext->sample_rate = AUDIO_SAM_RATE;
    codecContext->channels = 2;
    codecContext->channel_layout = AV_CH_LAYOUT_STEREO;  // 立体声
    codecContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    codecContext->bit_rate = 64000;
    codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    //打开音频的编码器
    int ret = avcodec_open2(codecContext, codec, nullptr);
    if (ret < 0) { LOG_ERROR("avcodec_open2 failed"); }

    swrContext = swr_alloc_set_opts(swrContext, codecContext->channel_layout,
                                    codecContext->sample_fmt,
                                    AUDIO_SAM_RATE,  //输出的音频参数
                                    AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                                    AUDIO_SAM_RATE,  //输入的音频参数
                                    0, 0);

    if (!swrContext) { LOG_ERROR("swr_alloc_set_opts failed"); }

    ret = swr_init(swrContext);
    if (ret < 0) { LOG_ERROR("swr_init failed"); }

    avFrame = av_frame_alloc();
    avFrame->format = AV_SAMPLE_FMT_FLTP;
    avFrame->channels = 2;
    avFrame->channel_layout = AV_CH_LAYOUT_STEREO;
    avFrame->nb_samples = 1024;  //一帧音频的样本数量

    ret = av_frame_get_buffer(avFrame, 0);
    if (ret < 0) { LOG_ERROR("av_frame_get_buffer failed"); }
}

/**
 * @brief 从buff所指向的内存取4096个字节进行编码
 *
 * @param buff 指向4096长度的pcm数据
 * @return ZipedFrame* 编码失败返回nullptr
 */
ZipedFrame *encodeFrame(void *buff) {
    int len = swr_convert(
            swrContext, avFrame->data, avFrame->nb_samples,  //重采样之后的数据
            (const uint8_t **) &buff, avFrame->nb_samples  //重采样之前的数据
    );
    if (len <= 0) { return nullptr; }
    AVPacket pkt;
    av_init_packet(&pkt);

    //将重采样的数据发送到编码线程
    int ret = avcodec_send_frame(codecContext, avFrame);
    if (ret != 0) { return nullptr; }

    ret = avcodec_receive_packet(codecContext, &pkt);
    if (ret != 0) { return nullptr; }

    write_aac_header(&pkt);
    return (new ZipedFrame(pkt.size, pkt.data));
}

void closeEncoder() {
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);
}
