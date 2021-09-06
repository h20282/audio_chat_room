#pragma once

#include <vector>

class AbstractAudioFrameProvider {
public:
    virtual std::vector<char> GetAudioFrame() = 0;
    virtual ~AbstractAudioFrameProvider() = default;
};
