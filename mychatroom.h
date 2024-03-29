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
#pragma once

#include "./common/IMToolBox.h"
#include "./login_register/login.h"
#include "./login_register/registerwin.h"
#include "./meet_room/roomdialog.h"
#include "./meet_room/useritem.h"
#include "./meet_room/userlist.h"
#include "./meet_room/userwidget.h"

#include "./net_api/qmytcp_client.h"
#include "./net_api/pack_def.h"
#include "./common/customwidget.h"

#include "audio/AudioChat.h"

#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMenu>
#include <QPoint>
#include <QCryptographicHash>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStringListModel>
#include <QStandardItemModel>

#include <QMap>
#include <map>
#include <string.h>
#include <list>
#include <QList>
#include <QTimer>

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
    void InitRoomDialogUi();
    void InitRoomListUi();
    void clearQuitRoomInfo(QString name);
    void releaseSource();
    void JoinRoom();
    QByteArray GetMD5(QString str);
    ~MyChatRoom();

signals:
    void SIG_createRoomSubmit(int user_id);         //创建房间信号
    void SIG_joinRoomSubmit(int room_num);           //加入房间信号
    void SIG_UnMute(int user_id);                    //用户向服务端发出解除静音信号信号
    void SIG_MuteOneUser(int user_id);               //关闭房间内指定用户的声音信号
    void SIG_AdjustUserVolume(int user_id);          //调节房间内指定用户的音量
    void SIG_UserSetting(int user_id);               //房间内进行用户界面设置
    void SIG_RefreshRoomList();                     //刷新房间列表

public slots:
    void HeartDetect();                                 //心跳检测槽函数
    void SLOT_loginSubmit(QString name, QString passwd); //登录槽函数，发送用户名、密码给服务端验证
    void SLOT_registerSubmit(QString name, QString passwd); //注册槽函数
    void SLOT_backToLoginInterface();                       //注册页面返回登录页面
    void SLOT_SkipLogin();                             //跳过登录界面，测试用。
    void SLOT_createRoomSubmit(int user_id); //创建房间槽函数
    void SLOT_joinRoomSubmit(int room_num); //加入房间槽函数
    void SLOT_quitRoomSubmit(); //退出房间槽函数 主动
    void SLOT_RefreshRoomList();    //刷新房间列表槽函数
    void SLOT_openAudio();      //打开声音
    void SLOT_closeAudio();     //关闭声音
    void SLOT_UnMute(); //用户自己被房主静音，解除静音。
    void SLOT_MuteOneUser(int mute_user_id); //关闭房间内指定用户的声音
    void SLOT_transferOneUser(int transfer_user_id); //转让房主
    void SLOT_kick_out_ofOneUser(int kick_user_id);  //踢人
    void SLOT_refreshUserList();        //刷新用户列表
    void SLOT_dealClientData(char* buf, int len); //客户端收到可读信号，触发SLOT_dealClientData槽函数，其中封装了下面的登录、注册等等各个槽函数。可用策略模式完成。
    void DealLoginResponse(char* buf, int len); //接收服务端的登录回复
    void DealRegisterResponse(char* buf, int len); //接收服务端的注册回复
    void DealCreateRoomResponse(char* buf, int len); //创建房间回复
    void DealJoinRoomResponse(char* buf, int len); //加入房间回复
    void DealRefreshRoomListHeader(char* buf, int len);   //更新房间列表头部字段
    void DealRefreshRoomList(char* buf, int len);   //更新房间列表
    void DealRefreshUserListHeader(char* buf, int len);   //更新用户列表头部字段
    void DealRefreshUserList(char* buf, int len);   //更新用户列表
    void DealQuitRoomResponse(char* buf, int len); //退出房间回复
    void DealMuteOneUserResponse(char* buf, int len);  //静音用户回复
    void DealtransferOneUserResponse(char* buf, int len);                 //转让房主回复
    void Dealkick_out_ofOneUserResponse(char* buf, int len);              //踢人回复 如果被踢的是自己，则退出房间
    void DealUnmuteQequest(char* buf, int len);   //静音,客户端实现
    void DealJumpToRegisterInterface();

private slots:
    void on_pb_search_clicked();
    void on_le_search_returnPressed();
    void on_ButtonCreateRoom_clicked();
    void on_pb_min_clicked();
    void on_pb_close_clicked();
    void on_ButtonRefreshRoomList_clicked();

private:
    Ui::MyChatRoom *ui;
    RoomDialog *m_roomdialog;
    UserListWidget *m_user_list_widget;
    RoomListWidget* m_room_widget;
    LOGIN *m_login;
    registerWin *m_register;

    QMyTcpClient *m_tcp_client;
    IMToolItem *m_userList;

    QTimer* timer;

    QString m_user_name;
    int m_user_id;
    int m_room_num;
    int m_room_owener_id;       //因为只能加入一个房间，因此给一个房主id，用于区分房主和普通用户的功能。
    QString m_room_owner_name;
    QAudioDeviceInfo m_device_info;
    bool is_muted;              //自己是否被静音
    bool is_first_connect;
    const QString m_server_ip;
    const unsigned short m_server_port;

    AudioChat *m_chat;
    AudioLevel m_level;

    QStandardItemModel userListModel;

    QMap<QString, UserWidget*> m_userWidegets;

};
