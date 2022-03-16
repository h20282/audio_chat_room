#pragma once;

#include "opus.h"

#include "types.h"

class ODecoder {
public:
    ODecoder(opus_int32 sample_rate, int channels);

    AudioData Decode(AudioData data);

private:
    std::shared_ptr<OpusDecoder> decoder_;
    opus_int32 sample_rate_;
    int channels_;
};
