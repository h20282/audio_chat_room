/*
 * @Author: FengYanBin
 * @Date: 2021-08-10 10:19:38
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-20 18:54:40
 * @Description: file content
 * @FilePath: /server/include/tcp_kernel.h
 */
#ifndef _TCPKERNEL_H
#define _TCPKERNEL_H

#include "tcp_net.h"
#include "mysql_server.h"
#include <signal.h>
#include <map>
#include <memory>

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

    void HeartDetect(int clientfd, char *szbuf, int nlen);

	//打印测试
	void printTest();

    //注册
    void Registerequest(int, char *, int);
    //登录
    void Loginequest(int, char *, int);

    //从数据库获取个人信息
    UserInfo *getUserInfoFromSql(int id);
    //用户列表 : 将 id = id的用户信息发给用户, 并且把这个人的信息再发给好友
    void SendUserList(int id, char *szbuf, int nlen);

    //房间列表发给用户
    void SendRoomList(int id);

    void SendMsgToOnlineClient(int id, char *szbuf, int nlen);
    void CreateRoomequest(int clientfd, char *szbuf, int nlen);
    void JoinRoomequest(int clientfd, char *szbuf, int nlen);
    void QuitRoomequest(int clientfd, char *szbuf, int nlen);
    void RefreshRoomList(int clientfd, char *szbuf, int nlen);
    void TransferUserRequset(int clientfd, char *szbuf, int nlen);
    void KickOutOfUserRequset(int clientfd, char *szbuf, int nlen);
    void MuteOneUserRequset(int clientfd, char *szbuf, int nlen);
    void UnMuteRequset(int clientfd, char *szbuf, int nlen);
    void InitRandom();
    void Offlineequest(int clientfd, char *szbuf, int nlen);

    void setThis()
    {
        pThis = this;
    }

private:
    CMysql *m_sql;
    TcpNet *m_tcp;

    static void worker(int sig);
    static TcpKernel *pThis;

    void Timer();

    map<int, UserInfo *> m_mapIDToUserInfo;                            //全部用户的信息,用户id和用户信息
    map<int, list<UserInfo *>> m_mapRoomIDToUserList;                  //房间号和房主用户信息
};

#endif
