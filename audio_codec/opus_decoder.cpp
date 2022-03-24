#include "opus_decoder.h"

ODecoder::ODecoder(opus_int32 sample_rate, int channels)
        : sample_rate_(sample_rate), channels_(channels) {
    int ec;
    auto decoder = opus_decoder_create(sample_rate_, channels_, &ec);
    if (ec != OPUS_OK) {
        // TODO
    }
    decoder_.reset(decoder, [](OpusDecoder *p) { opus_decoder_destroy(p); });
}

AudioData ODecoder::Decode(AudioData data) {
    auto result = std::make_shared<std::vector<uint8_t>>(data->size() * 30);
    auto len = opus_decode(decoder_.get(), data->data(), data->size(),
                           reinterpret_cast<opus_int16 *>(result->data()),
                           data->size() / 2 / channels_, 0);
    result->resize(len);
    return result;
}

AudioData ODecoder::Decode(const uint8_t *data, std::size_t len) {
    auto result = std::make_shared<std::vector<uint8_t>>(len * 30);
    auto re_len = opus_decode(decoder_.get(), data, len,
                              reinterpret_cast<opus_int16 *>(result->data()),
                              len / 2 / channels_, 0);
    result->resize(re_len);
    return result;
}
