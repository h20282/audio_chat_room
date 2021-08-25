/*
 * @Author: FengYanBin
 * @Date: 2021-08-09 16:30:04
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-20 20:52:10
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
#include <chrono>

#pragma once

const int kAddCount = 10;
const int kTimeOut = 600;
const int kListenCount = 128;
const int kEpollSize = 4096;
const int kIpSize = 16;
const int kSqlSize = 400;
const int kHeartDetectTime = 4;
const double kOffLineTime = 4;

//ip和端口
const unsigned short kServerPort = 9527; //tcp
const char *const kServerIp = "172.16.0.8";
const unsigned short kUdpServerPort = 9528; //udp

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
    kPackRoomMemberQesponse,

    kPackRoomListRefreshHeadRequest,
    kPackRoomListRefreshHeadResponse,
    kPackRoomListQesponse,

    kPackQuitRoomQequest,
    kPackQuitRoomQesponse,

    kPackUnmuteQequest,
    kPackUnmuteQesponse,

    kPackMuteOneUserQequest,
    kPackMuteOneUserQesponse,

    kPackTransferUserQequest,
    kPackTransferUserQesponse,

    kPackKickOutOfUserQequest,
    kPackKickOutOfUserQesponse,

    kPackOfflineRequest,
    kPackOfflineQesponse,

    kPackHeartDetect,
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


/////////////////////网络//////////////////////////////////////

const int kMaxSize = 60;
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
    std::chrono::steady_clock::time_point m_time;
} StructLoginRequest;

//登录回复
typedef struct StructLoginRespose
{
    StructLoginRespose()
    {
        m_pack_type = kPackLoginQesponse;
        memset(m_user_name, 0, kMaxSize);
    }
    PackType m_pack_type;       //包类型
    int m_user_id;              //用户id
    char m_user_name[kMaxSize]; //用户名
    int m_login_result;         //登录结果

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
        memset(m_room_owner_name, 0, kMaxSize);
    }
    PackType m_pack_type;     //包类型
    int m_create_room_result; //创建房间结果
    int m_room_id;
    int m_room_owner_id;
    char m_room_owner_name[kMaxSize]; //用户名
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
        m_join_id = 0;
        m_room_owner_id = 0;
        memset(m_room_owner_name, 0, kMaxSize);
        memset(m_join_name, 0, kMaxSize);
    }
    PackType m_pack_type;   //包类型
    int m_join_room_result; //加入房间结果
    int m_room_id;
    int m_join_id;
    int m_room_owner_id;
    char m_room_owner_name[kMaxSize]; //用户名
    char m_join_name[kMaxSize];
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
typedef struct StructRoomMemberRequset
{
    StructRoomMemberRequset()
    {
        m_nType = kPackRoomMemberQequest;
        m_user_id = 0;
        memset(m_sz_user, 0, kMaxSize);
    }
    PackType m_nType; //包类型
    int m_user_id;
    char m_sz_user[kMaxSize];

} StructRoomMemberRequset;

//房间成员回复
typedef struct StructRoomMemberResponse
{
    StructRoomMemberResponse()
    {
        m_nType = kPackRoomMemberQesponse;
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

//离线请求
typedef struct StructOffLineRequest
{
    StructOffLineRequest()
	{
		m_pack_type = kPackOfflineRequest;
		m_user_id = 0;
		memset(szUserName, 0, kMaxSize);
	}
	PackType m_pack_type;
	int m_user_id;
	char szUserName[kMaxSize];
} StructOffLineRequest;

//静音某人请求
typedef struct StructMuteUserRequest
{
    StructMuteUserRequest()
    {
        m_pack_type = kPackMuteOneUserQequest;
        m_user_id = 0;
        mute_user_id = 0;
        memset(szUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    int mute_user_id;
    char szUserName[kMaxSize];

} StructMuteUserRequest;

//静音某人回复
typedef struct StructMuteUserResponse
{
    StructMuteUserResponse()
    {
        m_pack_type = kPackMuteOneUserQesponse;
        m_user_id = 0;
        mute_user_id = 0;
        memset(szUserName, 0, kMaxSize);
        memset(muteUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    int mute_user_id;
    char szUserName[kMaxSize];
    char muteUserName[kMaxSize];
} StructMuteUserResponse;

//解除静音请求
typedef struct StructUnMuteRequest
{
    StructUnMuteRequest()
    {
        m_pack_type = kPackUnmuteQequest;
        m_user_id = 0;
        memset(szUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    char szUserName[kMaxSize];

} StructUnMuteRequest;

//解除静音回复
typedef struct StructUnMuteResponse
{
    StructUnMuteResponse()
    {
        m_pack_type = kPackUnmuteQesponse;
        m_user_id = 0;
        memset(szUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    char szUserName[kMaxSize];
} StructUnMuteResponse;

//转让房主请求
typedef struct StructTransferUserRequest
{
    StructTransferUserRequest()
    {
        m_pack_type = kPackTransferUserQequest;
        m_user_id = 0;
        transfer_user_id = 0;
        memset(szUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    int transfer_user_id;
    char szUserName[kMaxSize];
} StructTransferUserRequest;

//转让房主回复
typedef struct StructTransferUserResponse
{
    StructTransferUserResponse()
    {
        m_pack_type = kPackTransferUserQesponse;
        m_user_id = 0;
        transfer_user_id = 0;
        memset(szUserName, 0, kMaxSize);
        memset(transferUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    int transfer_user_id;
    char szUserName[kMaxSize];
    char transferUserName[kMaxSize];
} StructTransferUserResponse;

//踢出用户请求
typedef struct StructKickOutOfUserRequest
{
    StructKickOutOfUserRequest()
    {
        m_pack_type = kPackKickOutOfUserQequest;
        m_user_id = 0;
        kick_user_id = 0;
        memset(szUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    int kick_user_id;
    char szUserName[kMaxSize];
} StructKickOutOfUserRequest;

//踢出用户回复
typedef struct StructKickOutOfUserResponse
{
    StructKickOutOfUserResponse()
    {
        m_pack_type = kPackKickOutOfUserQesponse;
        m_user_id = 0;
        kick_user_id = 0;
        memset(szUserName, 0, kMaxSize);
        memset(kickrUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    int kick_user_id;
    char szUserName[kMaxSize];
    char kickrUserName[kMaxSize];
} StructKickOutOfUserResponse;

//心跳检测回复
typedef struct StructHeartDetectResponse
{
    StructHeartDetectResponse()
    {
        m_pack_type = kPackHeartDetect;
        m_user_id = 0;
        memset(szUserName, 0, kMaxSize);
    }
    PackType m_pack_type; //包类型
    int m_user_id;
    char szUserName[kMaxSize];
    std::chrono::steady_clock::time_point m_time;
} StructHeartDetectResponse;

typedef struct UserInfo
{
    UserInfo()
    {
        m_fd = 0;
        m_id = 0;
        m_state = 0;
        m_room_id = 0;
        is_muted = false;
        memset(m_user_name, 0, kMaxSize);
    }
    UserInfo(int fd, int id, int room_id)
    {
        m_fd = fd;
        m_id = id;
        m_state = 0; //心跳检测用
        m_room_id = room_id;
        is_muted = false;
        memset(m_user_name, 0, kMaxSize);
    }
    int m_fd;
    int m_id;
    int m_state;
    int m_room_id;
    bool is_muted; //是否被静音
    char m_user_name[kMaxSize];
} UserInfo;

#endif
