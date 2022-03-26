#include "opus_decoder.h"

#include <cmath>

namespace codec {

ODecoder::ODecoder(opus_int32 sample_rate, int channels)
        : sample_rate_(sample_rate), channels_(channels) {
    int ec;
    auto decoder = opus_decoder_create(sample_rate_, channels_, &ec);
    if (ec != OPUS_OK) { *reinterpret_cast<int *>(0) = 0; }
    decoder_.reset(decoder, [](OpusDecoder *p) { opus_decoder_destroy(p); });
}

AudioData ODecoder::Decode(AudioData data) {
    return Decode(data->data(), data->size());
}

AudioData ODecoder::Decode(const uint8_t *data, std::size_t len) {
    auto result = std::make_shared<std::vector<uint8_t>>(len * 100);
    auto re_len = opus_decode(decoder_.get(), data, len,
                              reinterpret_cast<opus_int16 *>(result->data()),
                              result->size() / 2 / channels_, 0);
    re_len = std::fmax(re_len, 0);
    result->resize(re_len * 2 * channels_);
    return result;
}

}  // namespace codec
