#ifndef ABSTRACTAUDIOFRAMEPROVIDER_H
#define ABSTRACTAUDIOFRAMEPROVIDER_H

#include "../net_api/pack_def.h"
class AbstractAudioFrameProvider
{
public:
    virtual AudioFrame getAudioFrame()=0;
};

#endif // ABSTRACTAUDIOFRAMEPROVIDER_H
