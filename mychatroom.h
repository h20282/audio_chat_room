/*
 * @Author: FengYanBin
 * @Date: 2021-08-05 15:00:11
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-09 16:23:51
 * @Description: file content
 * @FilePath: \sql\mychatroom.h
 */
/*
 * @Author: FengYanBin
 * @Date: 2021-08-05 15:00:11
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-05 16:31:23
 * @Description: file content
 * @FilePath: \sql\MyChatRoom.h
 */
#ifndef MYCHATROOM_H
#define MYCHATROOM_H

#include "./login_register/login.h"
#include "./login_register/registerwin.h"
#include "./meet_room/roomdialog.h"
#include "./net_api/udp_net.h"
#include "./net_api/qmytcp_client.h"
#include "./net_api/pack_def.h"
#include "./common/customwidget.h"
#include "IMToolBox.h"

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenu>
#include <QCryptographicHash>

#include <QMap>

namespace Ui
{
    class MyChatRoom;
}

class MyChatRoom : public CustomMoveWidget
{
    Q_OBJECT

public:
    explicit MyChatRoom(QWidget *parent = nullptr);

    void setServerIP();

    QByteArray GetMD5(QString str);

    ~MyChatRoom();

signals:
    void SIG_createRoomSubmit();         //创建房间信号

    void SIG_joinRoomSubmit(int room_num);           //加入房间信号

    void SIG_quitRoomSubmit(int room_num);           //退出房间信号

    void SIG_UnMute(int user_id);                    //用户向服务端发出解除静音信号信号

    void SIG_MuteOneUser(int user_id);               //关闭房间内指定用户的声音信号

    void SIG_AdjustUserVolume(int user_id);          //调节房间内指定用户的音量

public slots:
    void SLOT_loginSubmit(QString name, QString passwd); //登录槽函数，发送用户名、密码给服务端验证

    void SLOT_registerSubmit(QString name, QString passwd); //注册槽函数

    void SLOT_createRoomSubmit(); //创建房间槽函数

    void SLOT_joinRoomSubmit(int room_num); //加入房间槽函数

    void SLOT_quitRoomSubmit(int room_num); //退出房间槽函数

    void SLOT_openAudio();      //打开声音

    void SLOT_closeAudio();     //关闭声音

    void SLOT_UnMute(int user_id); //用户向服务端发出解除静音信号槽函数

    void SLOT_MuteOneUser(int user_id); //关闭房间内指定用户的声音

    void SLOT_AdjustUserVolume(int user_id); //调节房间内指定用户的音量

    void SLOT_dealClientData(char* buf, int len); //客户端收到可读信号，触发SLOT_dealClientData槽函数，其中封装了下面的登录、注册等等各个槽函数。可用策略模式完成。

    void DealLoginResponse(char* buf, int len); //接收服务端的登录回复

    void DealRegisterResponse(char* buf, int len); //接收服务端的注册回复

    void DealCreateRoomResponse(char* buf, int len); //创建房间回复

    void DealJoinRoomResponse(char* buf, int len); //加入房间回复

    void DealQuitRoomResponse(char* buf, int len); //退出房间回复

//    void DealUnmuteQequest();   //静音

    void DealAudioData(char* buf, int len); //接收服务端发来的数据进行混音等操作，调用音频处理模块的SLOT_PlayAudio()。

    void DealForceQuitRoom(char* buf, int len); //处理服务端发来的踢人处理结果,判断是否是自己。是自己调用退出房间函数。

    void DealClientQuitResponse(char* buf, int len); //服务器向客户端发出某个客户端退出信号,在槽函数中处理，将客户端在线列表中的退出用户清除。

private slots:

    void on_pb_search_clicked();

    void on_le_search_returnPressed();

    void on_ButtonCreateRoom_clicked();

    void on_ButtonSetting_clicked();

    void on_pb_min_clicked();

    void on_pb_close_clicked();


private:
    Ui::MyChatRoom *ui;
    RoomDialog *m_roomdialog;
    QMenu *m_MainMenu;
    LOGIN *m_login;
    registerWin *m_register;

    QMyTcpClient *m_tcp_client;
    UdpNet *m_udp_client;
    IMToolItem *m_userList;

    QString m_user_name;

    //    QMap<int , UserItem *> m_mapIDToUserItem;
    //    QMap<int , ChatDialog *> m_mapIDToChatDialog;
    //    QMap<int , VideoItem *> m_mapIDToVideoItem;
    //    QMap<int , Audio_Write*> m_mapIDToAudioWrite;

    //IMToolItem *m_videoList;
    int m_user_id;
    quint32 m_room_num;
    const QString m_server_ip;
    const unsigned short m_server_port;

};

#endif // MYCHATROOM_H
