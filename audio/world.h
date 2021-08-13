#ifndef WORLD_H
#define WORLD_H


#include<QIODevice>
#include<QAudioInput>
#include<QAudioOutput>
//#include <speex/include/speex.h>
 
#include <QDebug>
#include <QTimer>

//#include "./WebRtcMoudle/signal_processing_library.h"
//#include "./WebRtcMoudle/noise_suppression_x.h"
//#include "./WebRtcMoudle/noise_suppression.h"
//#include "./WebRtcMoudle/gain_control.h"
//#include "./WebRtc_Vad/webrtc_vad.h"

//NS  降噪
//vad 静音检测

const int SPEEX_FRAME_BYTE = 320;
const int SPEEX_QUALITY = 8;

const int DEF_READ_DENOISE = 0;
const int DEF_WRITE_DENOISE = 0;
const int DEF_TEL_RQ = 100;
const int DEF_TEL_RS = 101;
const int DEF_TEL_DATA = 101;
const int DEF_TEL_END = 101;


#define USE_WEBRTC 0
#define USE_SPEEX 0


#endif // WORLD_H
