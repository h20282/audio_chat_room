/*
 * @Author: FengYanBin
 * @Date: 2021-08-10 10:19:38
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-12 11:57:30
 * @Description: file content
 * @FilePath: /server/include/tcp_kernel.h
 */
#ifndef _TCPKERNEL_H
#define _TCPKERNEL_H

#include "tcp_net.h"
#include "mysql_server.h"
#include <map>

class TcpKernel;
typedef void (TcpKernel::*PFUN)(int, char *, int nlen);

typedef struct
{
    PackType m_type;
    PFUN m_pfun;
} ProtocolMap;

class TcpKernel : public IKernel
{
public:
    int Open();
    void Close();
    void DealData(int, char *, int);

    //注册
    void Registerequest(int, char *, int);
    //登录
    void Loginequest(int, char *, int);

    //从数据库获取个人信息
    UserInfo *getUserInfoFromSql(int id);
    //用户列表 : 将 id = id的用户信息发给用户, 并且把这个人的信息再发给好友
    void SendUserList(int id);

    //房间列表发给用户
    void SendRoomList(int id);

    void SendMsgToOnlineClient(int id, char *szbuf, int nlen);
    //void ChatperrorRequ(int clientfd, char *szbuf, int nlen);
    void CreateRoomequest(int clientfd, char *szbuf, int nlen);
    void JoinRoomequest(int clientfd, char *szbuf, int nlen);
    void QuitRoomequest(int clientfd, char *szbuf, int nlen);
    void RefreshRoomList(int clientfd, char *szbuf, int nlen);
    void AudioFrameequest(int clientfd, char *szbuf, int nlen);
    //void VideoFrameequest(int clientfd, char *szbuf, int nlen);
    void InitRandom();
    //void Offlineequest(int clientfd, char *szbuf, int nlen);

private:
    CMysql *m_sql;
    TcpNet *m_tcp;

    map<int, UserInfo *> m_mapIDToUserInfo;           //全部用户的信息
    map<int, list<UserInfo *>> m_mapRoomIDToUserList; //房间和房主用户信息
};

#endif
