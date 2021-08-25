#ifndef ABSTRACTAUDIOFRAMEPROVIDER_H
#define ABSTRACTAUDIOFRAMEPROVIDER_H
#include "../structs/AudioFrame.h"
#include <QAudioEncoderSettings>
class AbstractAudioFrameProvider
{
public:
    virtual AudioFrame getAudioFrame()=0;
};
#endif // ABSTRACTAUDIOFRAMEPROVIDER_H
