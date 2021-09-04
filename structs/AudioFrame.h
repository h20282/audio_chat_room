#pragma once
#include "Config.h"
#include "log/log.h"
struct AudioFrame {
    int len;  // 长度，按字节数
    char buff[kAudioFrameLen];
    double getMaxVolume() {
        if (kAudioSamSize == 16) {
            auto p = reinterpret_cast<short *>(buff);
            auto maxVol = p[0];
            int n_samples = len / 2;
            for (int i = 0; i < n_samples; ++i) {
                maxVol = std::max(maxVol, p[i]);
            }
            return double(maxVol) / 32768;
        } else {
            LOG_ERROR("not implement!!! AUDIO_SAM_SIZE: {}", kAudioSamSize);
            return 0;
        }
    }
    double getVolumeSum() {
        if (kAudioSamSize == 16) {
            auto p = reinterpret_cast<short *>(buff);
            double sum = 0;
            int n_samples = len / 2;
            for (int i = 0; i < n_samples; ++i) { sum += std::abs(p[i]); }
            return sum / 32768 / (n_samples);
        } else {
            LOG_ERROR("not implement!!! AUDIO_SAM_SIZE: {}", kAudioSamSize);
            return 0;
        }
    }
};
