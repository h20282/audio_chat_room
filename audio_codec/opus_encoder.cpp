#include "opus_encoder.h"

OEncoder::OEncoder(opus_int32 sample_rate, int channels)
        : sample_rate_(sample_rate), channels_(channels) {
    int ec;
    auto encoder = opus_encoder_create(sample_rate, channels,
                                       OPUS_APPLICATION_VOIP, &ec);
    if (ec != OPUS_OK) {
        // TODO
    }
    encoder_.reset(encoder, [](OpusEncoder *p) { opus_encoder_destroy(p); });
}

AudioData OEncoder::Encode(AudioData data) {
    AudioData result = std::make_shared<std::vector<uint8_t>>(data->size());
    auto len = opus_encode(
            encoder_.get(), reinterpret_cast<opus_int16 *>(data->data()),
            data->size() / 2 / channels_, result->data(), result->size());

    result->resize(len);
    return result;
}
