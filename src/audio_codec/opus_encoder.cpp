#include "opus_encoder.h"

namespace codec {

OEncoder::OEncoder(opus_int32 sample_rate, int channels)
        : sample_rate_(sample_rate), channels_(channels) {
    int ec;
    auto fs = static_cast<opus_int32>(sample_rate);
    auto encoder = opus_encoder_create(fs, channels, OPUS_APPLICATION_VOIP, &ec);
    if (ec != OPUS_OK) { *reinterpret_cast<int *>(0) = 0; }
    encoder_.reset(encoder, [](OpusEncoder *p) { opus_encoder_destroy(p); });
}

AudioData OEncoder::Encode(AudioData data) {
    AudioData result = std::make_shared<std::vector<uint8_t>>(data->size());
    auto len = opus_encode(encoder_.get(), reinterpret_cast<opus_int16 *>(data->data()),
                           data->size() / 2 / channels_, result->data(), result->size());
    if (len < 0) { return result; }
    result->resize(len);
    return result;
}

}  // namespace codec
