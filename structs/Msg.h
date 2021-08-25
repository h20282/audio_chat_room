#ifndef MSG_H
#define MSG_H

#include "AudioFrame.h"

struct Msg{
    char name[16];
    AudioFrame frame;
};

#endif // MSG_H
