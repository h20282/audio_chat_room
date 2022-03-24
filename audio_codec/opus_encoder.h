#pragma once;

#include "opus.h"

#include "types.h"

using AudioData = std::shared_ptr<std::vector<uint8_t>>;

class OEncoder {

public:
    OEncoder(opus_int32 sample_rate, int channels);

    AudioData Encode(AudioData data);

private:
    std::shared_ptr<OpusEncoder> encoder_;
    opus_int32 sample_rate_;
    int channels_;
};
