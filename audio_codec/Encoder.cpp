#include "Encoder.h"

#include "log/log.h"

void initEncoder();
std::vector<char> encodeFrame(void *buff);
void closeEncoder();

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
}

namespace {
constexpr int kEncoderInputBufferLen = 4096;

char aac_adts_header[7];
}  // namespace

///////////////////////////////////////begin of class Encoder definition
Encoder::Encoder() {
    initEncoder();
    this->curr_idx_ = 0;
    LOG_INFO("Encoder::Encoder() finished");
}

Encoder::~Encoder() {
    closeEncoder();
    LOG_INFO("Encoder::~Encoder() finished");
}

void Encoder::PushAudioFrame(std::vector<char> frame) {
    queue_.enqueue(frame);
}

std::vector<char> Encoder::GetZipedFrame() {
    LOG_INFO("call Encoder::GetZipedFrame()");
    int rest_bytes = 0;
    for (auto iter = queue_.begin(); iter != queue_.end(); ++iter) {
        rest_bytes += (*iter).size();
    }
    rest_bytes -= curr_idx_;

    if (rest_bytes >= kEncoderInputBufferLen) {
        LOG_INFO("rest_bytes({}) >= 4096 ", rest_bytes);
        /*      front                                    tail
         * [--------------] <- [--------------] <- [--------------]
         *           ^
         *           |
         *       m_currIdx
         */
        std::vector<char> buff(kEncoderInputBufferLen, 0);
        int bytedNeed = kEncoderInputBufferLen;
        int buffIdx = 0;

        while (true) {

            int front_rest_bytes = static_cast<int>(queue_.front().size()) - curr_idx_;
            if (front_rest_bytes < bytedNeed) {
                memcpy(&buff[static_cast<size_t>(buffIdx)],
                       &queue_.front()[0] + curr_idx_,
                       static_cast<size_t>(front_rest_bytes));
                buffIdx += front_rest_bytes;
                bytedNeed -= front_rest_bytes;
                queue_.dequeue();
                curr_idx_ = 0;
            } else {
                memcpy(&buff[static_cast<size_t>(buffIdx)],
                       &queue_.front()[0] + curr_idx_,
                       static_cast<size_t>(bytedNeed));
                curr_idx_ += bytedNeed;
                break;
            }
        }
        // return std::move(...)? moving a local object in a return statement
        // prevents copy elision
        return encodeFrame(&buff[0]);
    } else {
        LOG_INFO("rest_bytes({}) < 4096 return {", rest_bytes);
        return {};
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
    for (int i = 0; static_cast<size_t>(i) < sizeof(rates) / sizeof(rates[0]);
         ++i) {
        if (rates[i] == kAudioSamRate) {
            LOG_INFO("{}->{}", kAudioSamRate, rates[i]);
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
// namespace
namespace {
/**
 * @brief 向文件fp写入一个该pkt的ADTS HEADER， 如果fp==nullptr则输出到控制台
 *
 * @param pkt 该packet
 * @return int
 */
void write_aac_header(const AVPacket *pkt) {
    aac_adts_header[3] =
            static_cast<char>(((chanCfg & 3) << 6) + ((7 + pkt->size) >> 11));
    aac_adts_header[4] = static_cast<char>(((7 + pkt->size) & 0x7FF) >> 3);
    aac_adts_header[5] = static_cast<char>((((7 + pkt->size) & 7) << 5) + 0x1F);
}
}  // namespace

void initEncoder() {
    init_aac_header();
    // av_register_all();
    // avcodec_register_all();
    codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (!codec) { LOG_ERROR("avcodec_find_encoder failed"); }

    //配置编码器上下文
    codecContext = avcodec_alloc_context3(codec);

    if (!codecContext) { LOG_ERROR("avcodec_alloc_context3 failed"); }

    codecContext->sample_rate = kAudioSamRate;
    codecContext->channels = 2;
    codecContext->channel_layout = AV_CH_LAYOUT_STEREO;  // 立体声
    codecContext->sample_fmt = AV_SAMPLE_FMT_FLTP;
    codecContext->bit_rate = 64000;
    codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    //打开音频的编码器
    int ret = avcodec_open2(codecContext, codec, nullptr);
    if (ret < 0) { LOG_ERROR("avcodec_open2 failed ret={}", ret); }

    swrContext = swr_alloc_set_opts(swrContext, AV_CH_LAYOUT_STEREO,
                                    codecContext->sample_fmt,
                                    kAudioSamRate,  //输出的音频参数
                                    AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                                    kAudioSamRate,  //输入的音频参数
                                    0, nullptr);

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

std::vector<char> encodeFrame(void *buff) {
    int len = swr_convert(
            swrContext, avFrame->data, avFrame->nb_samples,  //重采样之后的数据
            (const uint8_t **) &buff, avFrame->nb_samples  //重采样之前的数据
    );
    if (len <= 0) {
        LOG_ERROR("swr_convert faild, code:{}", len);
        return {};
    }
    AVPacket pkt;
    av_init_packet(&pkt);

    //将重采样的数据发送到编码线程
    int ret = avcodec_send_frame(codecContext, avFrame);
    if (ret != 0) {
        LOG_ERROR("avcodec_send_frame faild, code:{}", ret);
        return {};
    }

    ret = avcodec_receive_packet(codecContext, &pkt);
    if (ret != 0) {
        LOG_INFO("avcodec_receive_packet faild, code{}", ret);
        return {};
    }

    write_aac_header(&pkt);
    std::vector<char> aac_data(7 + static_cast<size_t>(pkt.size), 0);
    memcpy(reinterpret_cast<void *>(&aac_data[0]), aac_adts_header, 7);
    memcpy(reinterpret_cast<void *>(&aac_data[7]), pkt.data,
           static_cast<size_t>(pkt.size));

    // return std::move(...)? moving a local object in a return statement
    // prevents copy elision
    return aac_data;
}

void closeEncoder() {
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);
}
