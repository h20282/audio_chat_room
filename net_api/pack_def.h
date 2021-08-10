/*
 * @Author: FengYanBin
 * @Date: 2021-08-09 16:30:04
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-09 16:38:42
 * @Description: file content
 * @FilePath: \sql\net_api\pack_def.h
 */
#ifndef __PACKDEF__
#define __PACKDEF__

#include <QHostAddress>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <winsock2.h>

const int DEF_PACK_BASE = 10000;
const int DEF_PACK_COUNT = 100;

//ip和端口
const unsigned short kServerPort = 10004;
const QString kServerIp = "119.91.116.26";

typedef enum NetPACKDef
{
    kPackRegisterQequest = 10000,
    kPackRegisterQesponse,

    kPackLoginQequest,
    kPackLoginQesponse,

    kPackCreateRoomQequest,
    kPackCreateRoomQesponse,

    kPackJoinRoomQequest,
    kPackJoinRoomQesponse,

    kPackQuitRoomQequest,
    kPackQuitRoomQesponse,

    kPackUnmuteQequest,
    kPackUnmuteQesponse,

    kPackMuteOneUserQequest,
    kPackMuteOneUserQesponse,

    kPackAdjustUserVolumeQequest,
    kPackAdjustUserVolumeQesponse,

    kPackAudioQequest,
    kPackAudioQesponse,
} Net_PACK;

//注册请求结果
const int kUserIdExist     = 0;
const int kRegisterSucess  = 1;

//登录请求结果
const int kUserIdNotExist  = 0;
const int kPasswdError     = 1;
const int kLoginSucess     = 2;
const int kUserOnLine      = 3;

//创建房间结果
const int kRoomIsExist     = 0;
const int kCreateSuccess   = 1;

//加入房间结果
const int kRoomNotExist    = 0;
const int kJoinSuccess     = 1;

//上传请求结果
#define file_is_exist        0
#define file_uploaded        1
#define file_uploadrq_sucess 2
#define file_upload_refuse   3

//上传回复结果
#define fileblock_failed     0
#define fileblock_success    1

//下载请求结果
#define file_downrq_failed   0
#define file_downrq_success  1

#define _downloadfileblock_fail  0
#define _downloadfileblock_success	1

const int kMaxPath          = 260;
const int kMaxSize          = 60;
const int kHobbyCount       = 8;
const int kMaxContentLength = 4096;

/////////////////////网络//////////////////////////////////////


const int kMaxBuffer        = 1024;
const int kBufferSize	    = 4096;

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

    PackType m_pack_type;   //包类型
    char     m_user_name[kMaxSize] ; //用户名
    char     m_user_passwd[kMaxSize];  //密码
}StructLoginRequest;

//登录回复
typedef struct StructLoginRespose
{
    StructLoginRespose()
    {
        m_pack_type= kPackLoginQesponse;
    }
    PackType m_pack_type;   //包类型
    int  m_user_id;     //用户id
    int  m_login_result ; //登录结果

}StructLoginRespose;


//注册请求
typedef struct StructRegisterRequest
{
    StructRegisterRequest()
    {
        m_pack_type = kPackRegisterQequest;
        memset(m_szUser,0,kMaxSize);
        memset(m_szPassword,0,kMaxSize);
    }

    PackType m_pack_type;   //包类型
    char     m_szUser[kMaxSize] ; //用户名
    char     m_szPassword[kMaxSize];  //密码

}StructRegisterRequest;

//注册回复
typedef struct StructRegisterRespose
{
    StructRegisterRespose()
    {
        m_pack_type= kPackRegisterQesponse;
    }
    PackType m_pack_type;   //包类型
    int  m_register_result ; //注册结果

}StructRegisterRespose;

//创建房间请求
typedef struct StructCreateRoomRequest
{
    StructCreateRoomRequest()
    {
        m_pack_type = kPackCreateRoomQequest;
        m_user_id = 0;
    }

    PackType m_pack_type;   //包类型
    int m_user_id;

}StructCreateRoomRequest;

//创建房间回复
typedef struct StructCreateRoomRespose
{
    StructCreateRoomRespose()
    {
        m_pack_type= kPackCreateRoomQesponse;
        m_create_room_result = 0;
        m_room_id = 0;
    }
    PackType m_pack_type;   //包类型
    int  m_create_room_result ;    //创建房间结果
    int  m_room_id;

}StructCreateRoomRespose;

//加入房间请求
typedef struct StructJoinRoomRequest
{
    StructJoinRoomRequest()
    {
        m_pack_type = kPackJoinRoomQequest;
        m_user_id = 0;
        m_room_id = 0;
    }

    PackType m_pack_type;   //包类型
    int m_user_id;
    int m_room_id;

}StructJoinRoomRequest;

//加入房间回复
typedef struct StructJoinRoomResponse
{
    StructJoinRoomResponse()
    {
        m_pack_type= kPackJoinRoomQesponse;
        m_join_room_result = 0;
        m_room_id = 0;
    }
    PackType m_pack_type;   //包类型
    int  m_join_room_result ;    //加入房间结果
    int m_room_id;
}StructJoinRoomResponse;

//退出房间请求
typedef struct StructQuitRoomRequest
{
    StructQuitRoomRequest()
    {
        m_pack_type = kPackQuitRoomQequest;
        m_user_id = 0;
        m_room_id = 0;
    }
    PackType   m_pack_type;   //包类型
    int    m_user_id; //用户ID
    int    m_room_id;
}StructQuitRoomRequest;

//退出房间回复
typedef struct StructQuitRoomResponse
{
    StructQuitRoomResponse()
    {
        m_pack_type = kPackQuitRoomQesponse;
        m_user_id = 0;
        memset(szUserName, 0, kMaxSize);
    }
    PackType   m_pack_type;   //包类型
    int m_user_id;
    char szUserName[kMaxSize];

}StructQuitRoomResponse;

typedef struct UserInfo
{
    UserInfo()
    {
         m_fd = 0;
         m_id = 0;
         m_state= 0;
         memset(m_user_name, 0 , kMaxSize);;
    }
    int  m_fd;
    int  m_id;
    int  m_state;
    char m_user_name[kMaxSize];
}UserInfo;


#endif __PACKDEF__
