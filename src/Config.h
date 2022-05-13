#pragma once
//#define kAudioSamRate 44100
//#define kAudioSamSize 16
//#define kAudioSamCount 2

// 1/25s(0.04s)的音频数据
//#define kAudioFrameLen kAudioSamRate*kAudioSamSize*kAudioSamCount/8/25

constexpr int kAudioSamRate = 48000;
constexpr int kAudioSamSize = 16;
constexpr int kAudioSamCount = 2;

constexpr int kLogRate = 100;

#define kPattern "[%Y-%m-%d %H:%M:%S.%e][%t][%l][%s][%!:%#]:%v"

#define SERVER_IP "119.91.116.26"
//#define SERVER_IP "192.168.11.129"
#define UDP_SERVER_PORT 9528
