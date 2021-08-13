/*
 * @Author: FengYanBin
 * @Date: 2021-08-09 16:30:04
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-12 15:44:46
 * @Description: file content
 * @FilePath: /server/include/pack_def.h
 */
#ifndef __PACKDEF_H_
#define __PACKDEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <malloc.h>
#include <sys/epoll.h>
#include <iostream>
#include <map>
#include <list>

#pragma once

const int kPackBase = 10000;
const int kPackCount = 100;

const int kAddCount = 10;
const int kTimeOut = 600;
const int kListenCount = 128;
const int kEpollSize = 4096;
const int kIpSize = 16;
const int kSqlSize = 400;

//ip和端口
const unsigned short kServerPort = 10004;
const char *const kServerIp = "192.168.201.129";

typedef enum NetPackDef
{
    kPackRegisterQequest = 10000,
    kPackRegisterQesponse,

    kPackLoginQequest,
    kPackLoginQesponse,

    kPackCreateRoomQequest,
    kPackCreateRoomQesponse,

    kPackJoinRoomQequest,
    kPackJoinRoomQesponse,

    kPackRoomMemberHeaderQequest,
    kPackRoomMemberQequest,

    kPackRoomListRefreshHeadRequest,
    kPackRoomListRefreshHeadResponse,
    kPackRoomListQesponse,

    kPackQuitRoomQequest,
    kPackQuitRoomQesponse,

    kPackUnmuteQequest,
    kPackUnmuteQesponse,

    kPackMuteOneUserQequest,
    kPackMuteOneUserQesponse,

    kPackAdjustUserVolumeQequest,
    kPackAdjustUserVolumeQesponse,

    kPackOfflineRequest,
    kPackOfflineQesponse,

    kPackAudioQequest,
    kPackAudioQesponse,

    kPackVideoQequest,
    kPackVideoQesponse,
} Net_Pack;

///////////////////请求结果///////////////////

//注册请求结果
const int kUserIdExist = 0;
const int kRegisterSucess = 1;

//登录请求结果
const int kUserIdNotExist = 0;
const int kPasswdError = 1;
const int kLoginSucess = 2;
const int kUserOnLine = 3;

//创建房间结果
const int kRoomIsExist = 0;
const int kCreateSuccess = 1;

//加入房间结果
const int kRoomNotExist = 0;
const int kJoinSuccess = 1;

//上传请求结果
#define file_is_exist 0
#define file_uploaded 1
#define file_uploadrq_sucess 2
#define file_upload_refuse 3

//上传回复结果
#define fileblock_failed 0
#define fileblock_success 1

//下载请求结果
#define file_downrq_failed 0
#define file_downrq_success 1

#define _downloadfileblock_fail 0
#define _downloadfileblock_success 1

const int kMaxPath = 260;
const int kMaxSize = 60;
const int kHobbyCount = 8;
const int kMaxContentLength = 4096;

/////////////////////网络//////////////////////////////////////

const int kMaxBuffer = 1024;
const int kBufferSize = 4096;

typedef int PackType;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//登录请求
typedef struct StructLoginRequest
{
    StructLoginRequest()
    {
        m_pack_type = kPackLoginQequest;
        memset(m_user_name, 0, kMaxSize);
        memset(m_user_passwd, 0, kMaxSize);
    }

    PackType m_pack_type;         //包类型
    char m_user_name[kMaxSize];   //用户名
    char m_user_passwd[kMaxSize]; //密码
} StructLoginRequest;

//登录回复
typedef struct StructLoginRespose
{
    StructLoginRespose()
    {
        m_pack_type = kPackLoginQesponse;
    }
    PackType m_pack_type; //包类型
    int m_user_id;        //用户id
    int m_login_result;   //登录结果

} StructLoginRespose;

//注册请求
typedef struct StructRegisterRequest
{
    StructRegisterRequest()
    {
        m_pack_type = kPackRegisterQequest;
        memset(m_szUser, 0, kMaxSize);
        memset(m_szPassword, 0, kMaxSize);
    }

    PackType m_pack_type;        //包类型
    char m_szUser[kMaxSize];     //用户名
    char m_szPassword[kMaxSize]; //密码

} StructRegisterRequest;

//注册回复
typedef struct StructRegisterRespose
{
    StructRegisterRespose()
    {
        m_pack_type = kPackRegisterQesponse;
    }
    PackType m_pack_type;  //包类型
    int m_register_result; //注册结果

} StructRegisterRespose;

//创建房间请求
typedef struct StructCreateRoomRequest
{
    StructCreateRoomRequest()
    {
        m_pack_type = kPackCreateRoomQequest;
        m_user_id = 0;
    }

    PackType m_pack_type; //包类型
    int m_user_id;

} StructCreateRoomRequest;

//创建房间回复
typedef struct StructCreateRoomRespose
{
    StructCreateRoomRespose()
    {
        m_pack_type = kPackCreateRoomQesponse;
        m_create_room_result = 0;
        m_room_id = 0;
        m_room_owner_id = 0;
    }
    PackType m_pack_type;     //包类型
    int m_create_room_result; //创建房间结果
    int m_room_id;
    int m_room_owner_id;
} StructCreateRoomRespose;

//加入房间请求
typedef struct StructJoinRoomRequest
{
    StructJoinRoomRequest()
    {
        m_pack_type = kPackJoinRoomQequest;
        m_user_id = 0;
        m_room_id = 0;
    }

    PackType m_pack_type; //包类型
    int m_user_id;
    int m_room_id;

} StructJoinRoomRequest;

//加入房间回复
typedef struct StructJoinRoomResponse
{
    StructJoinRoomResponse()
    {
        m_pack_type = kPackJoinRoomQesponse;
        m_join_room_result = 0;
        m_room_id = 0;
        m_room_owner_id = 0;
    }
    PackType m_pack_type;   //包类型
    int m_join_room_result; //加入房间结果
    int m_room_id;
    int m_room_owner_id;
} StructJoinRoomResponse;

//房间成员头部请求
typedef struct StructRoomMemberHeaderResponse
{
    StructRoomMemberHeaderResponse()
    {
        m_nType = kPackRoomMemberHeaderQequest;
        m_user_id = 0;
        memset(m_sz_user, 0, kMaxSize);
    }
    PackType m_nType; //包类型
    int m_user_id;
    char m_sz_user[kMaxSize];

} StructRoomMemberHeaderResponse;

//房间成员请求
typedef struct StructRoomMemberResponse
{
    StructRoomMemberResponse()
    {
        m_nType = kPackRoomMemberQequest;
        m_user_id = 0;
        memset(m_sz_user, 0, kMaxSize);
    }
    PackType m_nType; //包类型
    int m_user_id;
    char m_sz_user[kMaxSize];

} StructRoomMemberResponse;

//房间列表头部请求
typedef struct StructRoomListRefreshHeaderRequest
{
    StructRoomListRefreshHeaderRequest()
    {
        m_nType = kPackRoomListRefreshHeadRequest;
        m_user_id = 0;
    }
    PackType m_nType; //包类型
    int m_user_id;
} StructRoomListRefreshHeaderRequest;
//房间列表头部回复
typedef struct StructRoomListRefreshHeaderResponse
{
    StructRoomListRefreshHeaderResponse()
    {
        m_nType = kPackRoomListRefreshHeadResponse;
        m_user_id = 0;
    }
    PackType m_nType; //包类型
    int m_user_id;
} StructRoomListRefreshHeaderResponse;

//房间列表回复
typedef struct StructRoomListResponse
{
    StructRoomListResponse()
    {
        m_nType = kPackRoomListQesponse;
        m_room_num = 0;
        memset(m_owner_name, 0, kMaxSize);
    }
    PackType m_nType; //包类型
    int m_room_num;
    char m_owner_name[kMaxSize];
} StructRoomListResponse;

//退出房间请求
typedef struct StructQuitRoomRequest
{
    StructQuitRoomRequest()
    {
        m_pack_type = kPackQuitRoomQequest;
        m_user_id = 0;
        m_room_id = 0;
    }
    PackType m_pack_type; //包类型
    int m_user_id;        //用户ID
    int m_room_id;
} StructQuitRoomRequest;

//退出房间回复
typedef struct StructQuitRoomResponse
{
    StructQuitRoomResponse()
    {
        m_pack_type = kPackQuitRoomQesponse;
        m_user_id = 0;
        memset(szUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    char szUserName[kMaxSize];

} StructQuitRoomResponse;

typedef struct UserInfo
{
    UserInfo()
    {
        m_fd = 0;
        m_id = 0;
        m_state = 0;
        m_room_id = 0;
        memset(m_user_name, 0, kMaxSize);
    }
    UserInfo(int fd, int id, int room_id)
    {
        m_fd = fd;
        m_id = id;
        m_state = 0;
        m_room_id = room_id;
        memset(m_user_name, 0, kMaxSize);
    }
    int m_fd;
    int m_id;
    int m_state;
    int m_room_id;
    char m_user_name[kMaxSize];
} UserInfo;

#endif
