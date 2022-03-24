#pragma once;

#include "opus.h"

#include "types.h"

class ODecoder {
public:
    ODecoder(opus_int32 sample_rate, int channels);

    AudioData Decode(AudioData data);
    AudioData Decode(const uint8_t *data, std::size_t len);

private:
    std::shared_ptr<OpusDecoder> decoder_;
    opus_int32 sample_rate_;
    int channels_;
};
