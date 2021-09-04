#include "Decoder.h"

#include "log/log.h"

Decoder::Decoder() {
    this->InitDecoder();
}
Decoder::~Decoder() {
    this->CloseDecoder();
}

void Decoder::InitDecoder() {
    LOG_INFO("Decoder::InitDecoder()");

    cod_ = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (cod_ == nullptr) { LOG_ERROR("find codec fail"); }
    AVCodecParserContext *parser = av_parser_init(cod_->id);
    if (!parser) { LOG_ERROR("Parser not found"); }

    cod_ctx_ = avcodec_alloc_context3(cod_);
    if (cod_ctx_ == nullptr) {
        LOG_ERROR("Parser not found");
    } else {
        cod_ctx_->channels = 2;
        cod_ctx_->sample_rate = kAudioSamRate;
    }

    if (avcodec_open2(cod_ctx_, cod_, nullptr) < 0) {
        LOG_ERROR("Parser not found");
    }
    //创建packet,用于存储解码前的数据
    packet_ = (AVPacket *) malloc(sizeof(AVPacket));
    av_init_packet(packet_);

    //设置转码后输出相关参数
    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
    //采样个数
    int out_nb_samples = 1024;
    //采样格式
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    //采样率
    int out_sample_rate = kAudioSamRate;
    //通道数
    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);

    //创建buffer
    buffer_size_ = av_samples_get_buffer_size(
            nullptr, out_channels, out_nb_samples, AV_SAMPLE_FMT_S16, 1);

    //注意要用av_malloc
    buffer_ = (uint8_t *) av_malloc(buffer_size_);

    //创建Frame，用于存储解码后的数据
    frame_ = av_frame_alloc();

    int64_t in_channel_layout =
            av_get_default_channel_layout(cod_ctx_->channels);
    //打开转码器

    convert_ctx_ = swr_alloc();  //设置转码参数
    swr_alloc_set_opts(convert_ctx_, out_channel_layout, out_sample_fmt,
                       out_sample_rate, in_channel_layout, AV_SAMPLE_FMT_FLTP,
                       kAudioSamRate, 0, nullptr);
    //初始化转码器
    swr_init(convert_ctx_);
}

std::pair<unsigned char *, int> Decoder::DecodeFrame(void *buff, int len) {
    //    qDebug() << this << "is decoding...";
    if (buff == 0 || len == 0) { return std::make_pair(nullptr, 0); }
    packet_->data = (uint8_t *) buff;
    packet_->size = len;
    int ret = avcodec_send_packet(cod_ctx_, packet_);
    if (ret < 0) {
        char err_buff[128];
        av_strerror(ret, err_buff, sizeof(err_buff));
        LOG_ERROR("send_packet error code:{} '{%s}'", ret, err_buff);
        return std::make_pair(nullptr, 0);
    }

    ret = avcodec_receive_frame(cod_ctx_, frame_);
    if (ret < 0) { return std::make_pair(nullptr, 0); }

    //     * int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
    //                                const uint8_t **in , int in_count);

    //    frame->buf
    //    frame->data
    //    frame->pkt_size
    swr_convert(convert_ctx_, &buffer_, buffer_size_,
                (const uint8_t **) frame_->data, frame_->nb_samples);

    auto pcm = new unsigned char[buffer_size_];
    memcpy(pcm, buffer_, buffer_size_);

    return std::make_pair(pcm, buffer_size_);
}

void Decoder::CloseDecoder() {
    LOG_INFO("Decoder::CloseDecoder()");
    avcodec_close(cod_ctx_);
    av_frame_free(&frame_);
    //    av_free_packet(packet_);
    swr_free(&convert_ctx_);
}
