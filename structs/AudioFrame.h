#ifndef AUDIOFRAME_H
#define AUDIOFRAME_H
#include "Config.h"
#include <qdebug.h>
struct AudioFrame{
    int len; // 长度，按字节数
    char buff[AUDIO_FRAME_LEN];
    double getMaxVolume(){
        if (AUDIO_SAM_SIZE==16){
            auto p = reinterpret_cast<short*>(buff);
            auto maxVol = p[0];
            int nSamples = len/2;
            for ( int i=0; i<nSamples; i++ ) {
                maxVol = qMax(maxVol, p[i]);
            }
            return double(maxVol)/32768;
        } else {
            qDebug() << "not implement!!! AUDIO_SAM_SIZE: " << AUDIO_SAM_SIZE;
            return 0;
        }
    }
    double getVolumeSum(){
        if (AUDIO_SAM_SIZE==16){
            auto p = reinterpret_cast<short*>(buff);
            double sum = 0;
            int nSamples = len/2;
            for ( int i=0; i<nSamples; i++ ) {
                sum += qAbs(p[i]);
            }
            return sum/32768/(nSamples);
        } else {
            qDebug() << "not implement!!! AUDIO_SAM_SIZE: " << AUDIO_SAM_SIZE;
            return 0;
        }
    }
};
#endif // AUDIOFRAME_H
