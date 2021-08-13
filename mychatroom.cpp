/*
 * @Author: FengYanBin
 * @Date: 2021-08-05 15:00:11
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-09 21:17:08
 * @Description: file content
 * @FilePath: \sql\mychatroom.cpp
 */
#include "mychatroom.h"
#include "ui_mychatroom.h"

MyChatRoom::MyChatRoom(QWidget *parent) : CustomMoveWidget(parent),
                                          ui(new Ui::MyChatRoom),
                                          m_user_id(0), m_room_num(0), m_server_ip(kServerIp), m_server_port(kServerPort)
{
    ui->setupUi(this);
    ui->pb_title->setFocusPolicy(Qt::NoFocus);
    ui->le_search->setPlaceholderText("请输入6位房间号id！");
    ui->lb_name->setText(this->m_user_name);

    qDebug() << "tcp连接" << endl;

    //tcp连接
    m_tcp_client = new QMyTcpClient();
    m_tcp_client->InitNetWork(const_cast<char *>(m_server_ip.toStdString().c_str()), m_server_port);
    connect(m_tcp_client, SIGNAL(SIG_ReadyData(char *, int)), this, SLOT(SLOT_dealClientData(char *, int)));

    //登录界面显示
    m_login = new LOGIN();
    m_login->show();

    m_register = new registerWin();

    //客户端处理登录和注册请求
    //connect(m_login, &LOGIN::SIG_loginSubmit, this, &MyChatRoom::SLOT_loginSubmit);
    connect(m_login, SIGNAL(SIG_loginSubmit(QString, QString)), this, SLOT(SLOT_loginSubmit(QString, QString)));
    connect(m_login, SIGNAL(SIG_JumpToRegisterInterface()), this, SLOT(DealJumpToRegisterInterface()));
    connect(m_login, SIGNAL(SIG_SkipLogin()), this, SLOT(SLOT_SkipLogin()));
    connect(m_register, SIGNAL(SIG_registerSubmit(QString, QString)), this, SLOT(SLOT_registerSubmit(QString, QString)));
    connect(m_register, SIGNAL(SIG_backToLoginInterface()), this, SLOT(SLOT_backToLoginInterface()));

    //客户端处理房间内的请求
    m_roomdialog = new RoomDialog();

    InitRoomDialogUi();

    connect(m_roomdialog, SIGNAL(SIG_openAudio()), this, SLOT(SLOT_openAudio()));
    connect(m_roomdialog, SIGNAL(SIG_closeAudio()), this, SLOT(SLOT_closeAudio()));
    connect(m_roomdialog, SIGNAL(SIG_quitRoomSubmit()), this, SLOT(SLOT_quitRoomSubmit()));

    //客户端处理加入房间和创建房间请求
    connect(this, SIGNAL(SIG_joinRoomSubmit(int)), SLOT(SLOT_joinRoomSubmit(int)));
    connect(this, SIGNAL(SIG_createRoomSubmit(int)), this, SLOT(SLOT_createRoomSubmit(int)));
    connect(this, SIGNAL(SIG_RefreshRoomList()), this, SLOT(SLOT_RefreshRoomList()));
}

void MyChatRoom::InitRoomDialogUi()
{
    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    listWidget = new QListWidget(m_roomdialog);
    listWidget->move(globalPos.x() + 480, globalPos.y() + 88);
    listWidget->setFixedSize(300, 400);
    listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ProvideContextMenu(const QPoint &)));
}

QByteArray MyChatRoom::GetMD5(QString str)
{
    QByteArray md5 = QCryptographicHash::hash(str.toLatin1(), QCryptographicHash::Md5);
    return md5.toHex();
}

MyChatRoom::~MyChatRoom()
{
    delete ui;
}

void MyChatRoom::SLOT_loginSubmit(QString name, QString passwd)
{
    //发送给服务器
    qDebug() << "发给服务器登录请求！" << endl;
    StructLoginRequest rq;
    std::string strTmp = name.toStdString();
    char* buf = const_cast<char*>(strTmp.c_str());
    strcpy_s(rq.m_user_name, buf);


    //QByteArray ba = GetMD5(passwd);
    QByteArray ba = passwd.toUtf8();
    memcpy (rq.m_user_passwd, ba.data(), static_cast<size_t>(ba.length()));

    m_tcp_client->SendData(reinterpret_cast<char*>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_registerSubmit(QString name, QString passwd)
{
    qDebug() << "registerSubmit!" << endl;
    //发送给服务器
    StructRegisterRequest rq;
    std::string strTmp = name.toStdString();
    char* buf = const_cast<char*>(strTmp.c_str());
    strcpy_s(rq.m_user_name, buf);


    //QByteArray ba = GetMD5(passwd);
    QByteArray ba = passwd.toUtf8();
    memcpy(rq.m_user_passwd, ba.data(), static_cast<size_t>(ba.length()));

    m_tcp_client->SendData(reinterpret_cast<char*>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_backToLoginInterface()
{
    m_register->hide();
    m_login->show();
}

void MyChatRoom::SLOT_SkipLogin()
{
    //发送给服务器
    qDebug() << "管理员davy登录！" << endl;
    StructLoginRequest rq;
    std::string strTmp = "davy";
    char* buf = const_cast<char*>(strTmp.c_str());
    strcpy_s(rq.m_user_name, buf);

    QString passwd = "123";
    //QByteArray ba = GetMD5(passwd);
    QByteArray ba = passwd.toUtf8();
    memcpy (rq.m_user_passwd, ba.data(), static_cast<size_t>(ba.length()));

    m_tcp_client->SendData(reinterpret_cast<char*>(&rq), sizeof(rq));
    m_login->close();
    this->show();
}

void MyChatRoom::SLOT_createRoomSubmit(int user_id)
{
    qDebug() << "createRoomSubmit!" << endl;
    //发送给服务器
    StructCreateRoomRequest rq;
    rq.m_user_id = user_id;

    m_tcp_client->SendData(reinterpret_cast<char*>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_joinRoomSubmit(int room_num)
{
    qDebug() << "joinRoomSubmit!" << endl;
    //发送给服务器
    StructJoinRoomRequest rq;
    rq.m_user_id = this->m_user_id;
    rq.m_room_id = room_num;

    m_tcp_client->SendData(reinterpret_cast<char*>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_quitRoomSubmit()
{
    qDebug() << "user_id=!" << m_user_id << " " << "QuitRoomSubmit!" << endl;
    //发送给服务器
    StructQuitRoomRequest rq;
    rq.m_user_id = m_user_id;
    rq.m_room_id = m_room_num;

    m_tcp_client->SendData(reinterpret_cast<char*>(&rq), sizeof(rq));
    //m_roomdialog->close();
    //this->show();
}

void MyChatRoom::SLOT_RefreshRoomList()
{
    qDebug() << "SLOT_RefreshRoomList" << endl;
    StructRoomListRefreshHeaderRequest rq;
    rq.m_user_id = this->m_user_id;
    m_tcp_client->SendData(reinterpret_cast<char*>(&rq), sizeof(rq));
}

//打开音频
void MyChatRoom::SLOT_openAudio()
{
}

//关闭音频
void MyChatRoom::SLOT_closeAudio()
{
}

void MyChatRoom::SLOT_UnMute(int user_id)
{
}

void MyChatRoom::SLOT_MuteOneUser(int user_id)
{
}

void MyChatRoom::SLOT_AdjustUserVolume(int user_id)
{
}

void MyChatRoom::SLOT_dealClientData(char* buf, int len)
{
    int pkg_type = *(reinterpret_cast<int *>(buf)); //*(int*) 按四个字节取

    switch (pkg_type)
    {
    case kPackLoginQesponse:
        DealLoginResponse(buf, len);
        break;

    case kPackRegisterQesponse:
        DealRegisterResponse(buf, len);
        break;

    case kPackCreateRoomQesponse:
        DealCreateRoomResponse(buf, len);
        break;

    case kPackJoinRoomQesponse:
        DealJoinRoomResponse(buf, len);
        break;

    case kPackRoomListQesponse:
        DealRefreshRoomList(buf, len);
        break;

    case kPackRoomListRefreshHeadResponse:
        DealRefreshRoomListHeader(buf, len);
        break;

    case kPackRoomMemberQequest:
        DealRefreshUserList(buf, len);
        break;

    case kPackRoomMemberHeaderQequest:
        DealRefreshUserListHeader(buf, len);
        break;

    case kPackQuitRoomQesponse:
        DealQuitRoomResponse(buf, len);
        break;

    case kPackMuteOneUserQequest:;
        break;

    case kPackAdjustUserVolumeQequest:;
        break;

    case kPackAudioQequest:;
        break;
    }
}

void MyChatRoom::DealLoginResponse(char* buf, int len)
{
    StructLoginRespose * rs = reinterpret_cast<StructLoginRespose*>(buf);
    switch(rs->m_login_result)
    {
    case kUserIdNotExist:
        QMessageBox::information( this->m_login , "提示", "用户不存在, 登录失败"  );
        break;
    case kPasswdError:
        QMessageBox::information( this->m_login , "提示", "密码错误, 登录失败"  );
        break;
    case kLoginSucess:
        this->m_login->close();
        this->show();
        this->ui->lb_name->setText(m_user_name);
        this->setWindowTitle("多人语音聊天");
        this->m_user_id = rs->m_user_id;
        break;
    }
}

void MyChatRoom::DealRegisterResponse(char* buf, int len)
{
    StructRegisterRespose* rs = reinterpret_cast<StructRegisterRespose*>(buf);
    switch(rs->m_register_result)
    {
    case kUserIdExist:
        QMessageBox::information( this->m_login , "提示", "用户已存在, 注册失败"  );
        break;
    case kRegisterSucess:
        QMessageBox::information( this->m_login , "提示", "注册成功"  );
        break;
    }
}

void MyChatRoom::DealCreateRoomResponse(char* buf, int len)
{
    //解析
    StructCreateRoomRespose* rs = reinterpret_cast<StructCreateRoomRespose*>(buf);
    if (rs->m_create_room_result)//结果成功
    {//设置房间标志
        this->m_room_num = rs->m_room_id;
        this->m_room_owener_id = rs->m_room_owner_id;

        //房间显示以及设置
        this->hide();
        m_roomdialog->setRoomid(rs->m_room_id);
        m_roomdialog->show();

        //创建音频采集
        //m_pAudioRead = new Audio_Read;
//        connect(m_pAudioRead, SIGNAL(sig_net_tx_frame(QByteArray)) ,
//                this , SLOT(slot_sendAudioFrame(QByteArray)) );
    }
}

void MyChatRoom::DealJoinRoomResponse(char* buf, int len)
{
    //解析
    StructJoinRoomResponse* rs = reinterpret_cast<StructJoinRoomResponse*>(buf);
    if (rs->m_join_room_result)//结果成功
    {//设置房间标志
        this->m_room_num = rs->m_room_id;
        this->m_room_owener_id = rs->m_room_owner_id;

        qDebug() << "进入房间:" << len << endl;
        //房间显示以及设置
        this->hide();
        m_roomdialog->setRoomid(rs->m_room_id);
        m_roomdialog->show();
//        //创建音频采集
//        m_pAudioRead = new Audio_Read;
//        connect(m_pAudioRead, SIGNAL(sig_net_tx_frame(QByteArray)) ,
//                this , SLOT(slot_sendAudioFrame(QByteArray)) );
    }
}

void MyChatRoom::DealRefreshRoomListHeader(char *buf, int len)
{
    qDebug() << "DealRefreshRoomListHeader!" << endl;
    //StructRoomListRefreshHeaderResponse* rs = reinterpret_cast<StructRoomListRefreshHeaderResponse*>(buf);
    ui->RoomListWidget->clear();
}

//TODO：优化，现在每次都要全部刷新，只改发生变化的即可
void MyChatRoom::DealRefreshRoomList(char *buf, int len)
{
    qDebug() << "刷新房间列表！" << endl;
    StructRoomListResponse* rs = reinterpret_cast<StructRoomListResponse*>(buf);
    //this->m_mapRoomIDToUserList[rq->m_room_num] = rq->m_owner_name;

    //TODO:显示房间列表到客户端，要求显示房间号，房主。房主就是用户列表的头部。
    int num = rs->m_room_num;
    QString name = rs->m_owner_name;
    QString search = QString("    %1'                            '%2'").arg(num).arg(name);
    ui->RoomListWidget->addItem(search);
}

void MyChatRoom::DealRefreshUserListHeader(char *buf, int len)
{
    qDebug() << "清空用户列表！" <<  endl;
    StructRoomMemberHeaderResponse* rs = reinterpret_cast<StructRoomMemberHeaderResponse*>(buf);
    //TODO:清空用户列表
    int count = listWidget->count();
    qDebug() << "count=" << count << endl;
    for (int i = 0; i < count; ++i)
        listWidget->takeItem(0);


}

void MyChatRoom::DealRefreshUserList(char *buf, int len)
{
    qDebug() << "更新用户列表！" <<  endl;
    StructRoomMemberResponse* rs = reinterpret_cast<StructRoomMemberResponse*>(buf);
    int user_id = rs->m_user_id;
    QString name = rs->m_sz_user;
    QString text = QString("    '%1'         '%2'").arg(user_id).arg(name);

    QListWidgetItem* item = new QListWidgetItem(listWidget, 0);
    item->setSizeHint(QSize(50, 50));
    item->setText(text);

//    UserListWidgetItem *item = new UserListWidgetItem(listwidget);
//    item->setSizeHint(QSize(50, 50));
//    item->setText(text);


}

//这里有个逻辑问题，其实服务器应该返回给数据才关闭。但是客户端自己关闭就够了。
void MyChatRoom::DealQuitRoomResponse(char* buf, int len)
{
    qDebug() << "退出房间！" << endl;
    //解析包
    StructQuitRoomResponse* rs = reinterpret_cast<StructQuitRoomResponse*>(buf);
//    //清除视频模块
//    auto ite = m_mapIDToVideoItem.find(rs->m_UserID);
//    if( ite != m_mapIDToVideoItem.end() )
//    {
//        VideoItem *item = m_mapIDToVideoItem[rs->m_UserID];
//        m_videoList->removeItem( item);//界面去销毁
//        delete item;
//        m_mapIDToVideoItem.erase(ite);
//    }
//    //清除音频模块
//    auto iteAudio = m_mapIDToAudioWrite.find(rs->m_UserID);
//    if( iteAudio != m_mapIDToAudioWrite.end() )
//    {
//        Audio_Write *item = m_mapIDToAudioWrite[rs->m_UserID];
//        delete item;
//        m_mapIDToAudioWrite.erase(iteAudio);
//    }
    //如果回复的用户id是自己，那么关闭房间框
    if (rs->m_user_id == this->m_user_id) {

        int count = listWidget->count();
        qDebug() << "退出房间的人界面的count=" << count << endl;
        for (int i = 0; i < count; ++i)
            listWidget->takeItem(0);

        m_roomdialog->close();
        this->show();
    }
}

void MyChatRoom::DealAudioData(char* buf, int len)
{
}

void MyChatRoom::DealForceQuitRoom(char* buf, int len)
{
}

void MyChatRoom::DealClientQuitResponse(char* buf, int len)
{
}

void MyChatRoom::DealJumpToRegisterInterface()
{
    m_login->close();
    m_register->show();
}

void MyChatRoom::on_pb_search_clicked()
{
    int room_num = ui->le_search->text().toInt();
    emit SIG_joinRoomSubmit(room_num);
}

void MyChatRoom::on_le_search_returnPressed()
{
    int room_num = ui->le_search->text().toInt();
    emit SIG_joinRoomSubmit(room_num);
}

void MyChatRoom::on_ButtonCreateRoom_clicked()
{
    emit SIG_createRoomSubmit(m_user_id);
}

void MyChatRoom::on_ButtonSetting_clicked()
{
    //TODO:弹出设置界面
}

void MyChatRoom::on_pb_min_clicked()
{
    this->showMinimized();
}

void MyChatRoom::on_pb_close_clicked()
{
    if (QMessageBox::question(this, "提示", "是否退出应用?") == QMessageBox::Yes)
    {
        this->close();
    }
}

void MyChatRoom::on_pushButton_clicked()
{
    int num = 123456;
    QString name = "davy";
    QString search = QString("    %1'                            '%2'").arg(num).arg(name);
    //QString str = ""
    ui->RoomListWidget->addItem(search);
}

void MyChatRoom::on_pushButton_2_clicked()
{
    int num = 123455;
    QString name = "davy";
    QString search = QString("    %1'                            '%2'").arg(num).arg(name);
    //QString str = ""
    QListWidgetItem* item = new QListWidgetItem("123");
    ui->RoomListWidget->insertItem(3, item);
}

void MyChatRoom::on_pushButton_3_clicked()
{
    ui->RoomListWidget->takeItem(0);
}

void MyChatRoom::on_ButtonRefreshRoomList_clicked()
{
    emit SIG_RefreshRoomList();
}

void MyChatRoom::ProvideContextMenu(const QPoint &pos)
{
    QPoint item = listWidget->mapToGlobal(pos);
    QMenu submenu;
    submenu.addAction("ADD");
    submenu.addAction("Delete");
    //QAction* rightClickItem = submenu.exec(item);
    if(listWidget->itemAt(mapFromGlobal(QCursor::pos())) != NULL) //如果有item则添加"修改"菜单 [1]*
    {
        {
            submenu.addAction(new QAction("修改", listWidget));
        }
    }
    submenu.exec(QCursor::pos());
//    if (rightClickItem && rightClickItem->text().contains("davy") )
//    {
//        listWidget->takeItem(listWidget->indexAt(pos).row());
//    }
}



