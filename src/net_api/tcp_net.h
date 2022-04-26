#pragma once

#include <QObject>
#include <QQueue>
#include <QUdpSocket>
#include <QHostAddress>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include <process.h>
#include <winsock2.h>

class ITcpMediator
{
public:
    virtual void DealData(char* szbuf , int nLen) = 0;
    virtual ~ITcpMediator(){}
};

class TcpNet
{
public:
    TcpNet(ITcpMediator *pMediator);
    ~TcpNet();
public:
     bool InitNetWork(char* szBufIP , unsigned short port);

     void UnInitNetWork();
     int SendData(char* szbuf,int nLen);
     int ConnectToServer( char* szBufIP);
public:
    static unsigned __stdcall ThreadProc(void*);
public:
    SOCKET m_sockClient;
    HANDLE m_hThread;
    bool   m_bFlagQuit;
    unsigned short m_port;
    ITcpMediator *m_pMediator;
};
