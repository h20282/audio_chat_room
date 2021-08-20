#ifndef UDP_NET_H
#define UDP_NET_H

#include <QObject>
#include <QQueue>
#include <QUdpSocket>
#include <QHostAddress>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>
#include <unordered_set>

#include "net_api/pack_def.h"
//#include "./mychatroom.h"

extern QString g_userName;

// 从collector中获取音频，通过udp传输到服务器，接收服务器中的数据，通过 getAudioFrame() 提供出去
class UdpNet : public QObject
{
    Q_OBJECT
public:
    UdpNet();
    ~UdpNet(){ CloseNet(); }

    void changeMuteState();             //静音状态变化

    void MuteUser();

    void UnMuteUser();

    bool getIsMuted();                  //获取是否被静音

    void insertMuteUser(std::string name);              //在set中插入静音用户

    void delMuteUser(std::string name);

    virtual bool InitNet( ); //资源的创建包括 网络库的加载 创建socket 及conn_info 打开线程 以及线程处理
    virtual void CloseNet();
    virtual bool SendData(QByteArray data, QString IP);

    bool SendBroadCast(QByteArray data);

    void setRoomId(int room_id);

signals:
    void SIG_oneMsgReady(Msg msg);
    void SIG_oneEmptyFrameReady(QString name);

public slots:
    void onAudioFrameReady(AudioFrame frame);

private slots:


    void onUdpReadyRead();

private:
    //QQueue<AudioFrame> m_queue;

    QUdpSocket *m_udpSocket;
    QHostAddress m_destaddr;
   // MyChatRoom* m_chat_room;
    quint16 port_;

    //AudioCollector *m_collector;

    QMutex m_mutex;


    int m_roomId;
    bool m_isMuted = false;
    std::unordered_set<std::string> m_setMuteUsers;      //被静音的用户列表
};

#endif // UDP_NET_H
