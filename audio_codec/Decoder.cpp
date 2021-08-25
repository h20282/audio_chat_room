#include "Decoder.h"



//g++ AACtoPCM.cpp -o AACtoPCM.exe -ID:/ffmpeg/ffmpeg-4.0.2-win64-dev/include  -D__STDC_CONSTANT_MACROS -LD:/ffmpeg/ffmpeg-4.0.2-win64-dev/lib -lavcodec -lswresample -lavutil -lavformat -lswscale -std=c++11;


// 用于从aac文件中一帧一帧读取音频
struct aacFrameReader
{
    FILE *fp;
    unsigned char l = 0x01;
    unsigned char r = 0x01;
    aacFrameReader(const char* fileName){
        fp = fopen(fileName, "rb");
        if (!fp){
            cout << "line: " << __LINE__ << " [] = "<< endl;
        }
    }
    ~aacFrameReader(){
        fclose(fp);
    }
    std::pair<unsigned char*, int> getAacFrame () {
        static unsigned char buff[1024];
        memset(buff, 0, sizeof(buff));
        fread(buff, 1, 7, fp);

        cout << endl;
//        cout << std::bitset<8>(buff[3]) << " " << bitset<8>(buff[4]) << " " << bitset<8>(buff[5]) << endl;
        int len = ( (buff[3]&0x03)<<(8+3) ) + ( buff[4]<<3 ) + (buff[5]>>5);
        cout << "line: " << __LINE__ << " [len] = " << len << endl;
        fread(buff+7, 1, len-7, fp);

        return std::make_pair(buff, len);
    }

};

static AVPacket 		*packet = NULL;
static AVCodecContext  *cod_ctx = NULL;
static AVCodec         *cod = NULL;
static SwrContext 		*convert_ctx = NULL;
static AVFrame 		*frame = NULL;
static uint8_t 		*buffer = NULL;
static int 			buffer_size = 0;

void initDecoder(){
    av_register_all();

    // AVFormatContext *fmt_ctx = NULL;


    cod = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (cod == NULL)
        printf("find codec fail");
    AVCodecParserContext *parser = av_parser_init(cod->id);
    if (!parser) {
        printf("Parser not found\n");
    }

    cod_ctx = avcodec_alloc_context3(cod);
    if (cod_ctx ==  NULL)
        printf("alloc context fail");
    else {
        cod_ctx->channels = 2;
        cod_ctx->sample_rate = AUDIO_SAM_RATE;
    }

    if (avcodec_open2(cod_ctx, cod, NULL) < 0)
        printf("can't open codec");


    //创建packet,用于存储解码前的数据
    packet = (AVPacket *)malloc(sizeof(AVPacket));
    av_init_packet(packet);

    //设置转码后输出相关参数
    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
    //采样个数
    int out_nb_samples = 1024;
    //采样格式
    enum AVSampleFormat  out_sample_fmt = AV_SAMPLE_FMT_S16;
    //采样率
    int out_sample_rate = AUDIO_SAM_RATE;
    //通道数
    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
    printf("%d\n", out_channels);

    //创建buffer
    buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, AV_SAMPLE_FMT_S16, 1);

    //注意要用av_malloc
    buffer = (uint8_t *)av_malloc(buffer_size);

    //创建Frame，用于存储解码后的数据
    frame = av_frame_alloc();

    int got_picture;
    int64_t in_channel_layout = av_get_default_channel_layout(cod_ctx->channels);
    //打开转码器

    convert_ctx = swr_alloc();\
    //设置转码参数
    swr_alloc_set_opts(convert_ctx, out_channel_layout, out_sample_fmt, out_sample_rate, \
        in_channel_layout, AV_SAMPLE_FMT_FLTP, AUDIO_SAM_RATE, 0, NULL);
    //初始化转码器
    swr_init(convert_ctx);
}


std::pair<unsigned char*, int> decodeFrame(void*buff, int len){
    if (buff==0 || len==0){
        return std::make_pair(nullptr, 0);
    }
    packet->data = (uint8_t*)buff;
    packet->size = len;
    int ret = avcodec_send_packet(cod_ctx, packet);
    if (ret < 0) {
        printf("Error submitting the packet to the decoder\n");
        return std::make_pair(nullptr, 0);
    }

    ret = avcodec_receive_frame(cod_ctx, frame);
    if (ret<0){
        return std::make_pair(nullptr, 0);
    }

//     * int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
//                                const uint8_t **in , int in_count);


//    frame->buf
//    frame->data
//    frame->pkt_size
    swr_convert(convert_ctx, &buffer, buffer_size, (const uint8_t **)frame->data, frame->nb_samples);
//    printf("pts:%10lld\t packet size:%d\n", packet->pts, packet->size);


    auto pcm = new unsigned char[buffer_size];
    memcpy(pcm, buffer, buffer_size);

    return std::make_pair(pcm, buffer_size);

}


void closeDecoder(){
    // avformat_close_input(&fmt_ctx);
    avcodec_close(cod_ctx);
    av_frame_free(&frame);

    av_free_packet(packet);
    swr_free(&convert_ctx);
}





/////////////////////////////////////////////////////class version


Decoder::Decoder(){
    this->initDecoder();
}
Decoder::~Decoder(){
    this->closeDecoder();
}

void Decoder::initDecoder(){
    qDebug() << "decoder" << this << "init";
    av_register_all();

    // AVFormatContext *fmt_ctx = NULL;


    cod = avcodec_find_decoder(AV_CODEC_ID_AAC);
    if (cod == NULL)
        printf("find codec fail");
    AVCodecParserContext *parser = av_parser_init(cod->id);
    if (!parser) {
        printf("Parser not found\n");
    }

    cod_ctx = avcodec_alloc_context3(cod);
    if (cod_ctx ==  NULL)
        printf("alloc context fail");
    else {
        cod_ctx->channels = 2;
        cod_ctx->sample_rate = AUDIO_SAM_RATE;
    }

    if (avcodec_open2(cod_ctx, cod, NULL) < 0)
        printf("can't open codec");


    //创建packet,用于存储解码前的数据
    packet = (AVPacket *)malloc(sizeof(AVPacket));
    av_init_packet(packet);

    //设置转码后输出相关参数
    uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
    //采样个数
    int out_nb_samples = 1024;
    //采样格式
    enum AVSampleFormat  out_sample_fmt = AV_SAMPLE_FMT_S16;
    //采样率
    int out_sample_rate = AUDIO_SAM_RATE;
    //通道数
    int out_channels = av_get_channel_layout_nb_channels(out_channel_layout);
    printf("%d\n", out_channels);

    //创建buffer
    buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, AV_SAMPLE_FMT_S16, 1);

    //注意要用av_malloc
    buffer = (uint8_t *)av_malloc(buffer_size);

    //创建Frame，用于存储解码后的数据
    frame = av_frame_alloc();

    int got_picture;
    int64_t in_channel_layout = av_get_default_channel_layout(cod_ctx->channels);
    //打开转码器

    convert_ctx = swr_alloc();\
    //设置转码参数
    swr_alloc_set_opts(convert_ctx, out_channel_layout, out_sample_fmt, out_sample_rate, \
        in_channel_layout, AV_SAMPLE_FMT_FLTP, AUDIO_SAM_RATE, 0, NULL);
    //初始化转码器
    swr_init(convert_ctx);
}


std::pair<unsigned char*, int> Decoder::decodeFrame(void*buff, int len){
//    qDebug() << this << "is decoding...";
    if (buff==0 || len==0){
        return std::make_pair(nullptr, 0);
    }
    packet->data = (uint8_t*)buff;
    packet->size = len;
    int ret = avcodec_send_packet(cod_ctx, packet);
    if (ret < 0) {
        char err_buff[128];
//        int av_strerror(int errnum, char *errbuf, size_t errbuf_size);
        av_strerror(ret, err_buff, sizeof(err_buff));
        printf("Error submitting the packet to the decoder, err code:%d '%s'\n", ret, err_buff);
        return std::make_pair(nullptr, 0);
    }

    ret = avcodec_receive_frame(cod_ctx, frame);
    if (ret<0){
        return std::make_pair(nullptr, 0);
    }

//     * int swr_convert(struct SwrContext *s, uint8_t **out, int out_count,
//                                const uint8_t **in , int in_count);


//    frame->buf
//    frame->data
//    frame->pkt_size
    swr_convert(convert_ctx, &buffer, buffer_size, (const uint8_t **)frame->data, frame->nb_samples);
//    printf("pts:%10lld\t packet size:%d\n", packet->pts, packet->size);


    auto pcm = new unsigned char[buffer_size];
    memcpy(pcm, buffer, buffer_size);

    return std::make_pair(pcm, buffer_size);

}


void Decoder::closeDecoder(){
    qDebug() << "decoder" << this << "close";
    // avformat_close_input(&fmt_ctx);
    avcodec_close(cod_ctx);
    av_frame_free(&frame);

    av_free_packet(packet);
    swr_free(&convert_ctx);
}
