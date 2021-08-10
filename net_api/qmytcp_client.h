#ifndef QMYTCP_CLIENT_H
#define QMYTCP_CLIENT_H

#include <QObject>
#include"tcp_net.h"

class QMyTcpClient : public QObject,public ITcpMediator
{
    Q_OBJECT
public:
    explicit QMyTcpClient(QObject *parent = nullptr);
    ~QMyTcpClient();
signals:
    void SIG_ReadyData(char* szbuf, int nLen);
public slots:
    bool InitNetWork(char* szBufIP , unsigned short port);
    void UnInitNetWork();
    int SendData(char* szbuf,int nLen);
    int ConnectToServer( char* szBufIP);
    void DealData(char* szbuf , int nLen);
public:
    TcpNet *m_pTcp;
};

#endif // QMYTCP_CLIENT_H
