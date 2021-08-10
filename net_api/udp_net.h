#ifndef UDP_NET_H
#define UDP_NET_H

#include <QObject>
#include <QQueue>
#include <QUdpSocket>
#include <QHostAddress>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

const quint16 PORT = 10004;
const QString SERVERIP = "119.91.116.26";

// 从collector中获取音频，通过udp传输到服务器，接收服务器中的数据，通过 getAudioFrame() 提供出去
class UdpNet : public QThread
{
public:
    explicit UdpNet();
    ~UdpNet(){ CloseNet(); }

    virtual bool InitNet( ); //资源的创建包括 网络库的加载 创建socket 及conn_info 打开线程 以及线程处理
    virtual void CloseNet();
    virtual bool SendData(QByteArray data, QString IP);

    bool SendBroadCast(QByteArray data);

    void run() override;

private slots:
    void onUdpReadyRead();

private:
    //QQueue<AudioFrame> m_queue;

    QUdpSocket *m_udpSocket;
    QHostAddress m_destaddr;
    quint16 port_;

    //AudioCollector *m_collector;

    QMutex m_mutex;
};

#endif // UDP_NET_H
