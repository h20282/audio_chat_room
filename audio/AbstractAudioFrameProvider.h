#pragma once

#include "structs/AudioFrame.h"

class AbstractAudioFrameProvider {
public:
    virtual AudioFrame GetAudioFrame() = 0;
    virtual ~AbstractAudioFrameProvider() = default;
};
