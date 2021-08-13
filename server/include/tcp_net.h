/*
 * @Author: FengYanBin
 * @Date: 2021-08-10 10:19:38
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-10 15:40:16
 * @Description: file content
 * @FilePath: /server/include/tcp_net.h
 */
#ifndef _TCPNET_H
#define _TCPNET_H

#include "thread_pool.h"
#include "pack_def.h"
class IKernel
{
public:
    virtual void DealData(int, char *, int) = 0;
};

class TcpNet
{
public:
    TcpNet(IKernel *kernel) { m_kernel = kernel; }

    int InitNetWork();
    void UnInitNetWork();
    static void *EPOLL_Jobs(void *);
    static void *Accept_Deal(void *);
    static void *Info_Recv(void *);
    int SendData(int, char *, int);
    void Addfd(int, int);
    void Deletefd(int);
    void Epoll_Deal(int, pool_t *);

    void SetpThis(TcpNet *tcp) { m_pThis = tcp; }

public:
    struct epoll_event epollarr[kEpollSize];
    int sockfd;
    int clientfd;
    int epfd;

    pthread_mutex_t alock;
    pthread_mutex_t rlock;

    struct sockaddr_in serveraddr;
    IKernel *m_kernel;

    ThreadPool *m_pool;
    static TcpNet *m_pThis;
};

#endif
