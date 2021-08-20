/*
 * @Author: FengYanBin
 * @Date: 2021-08-05 15:00:11
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-18 09:29:08
 * @Description: file content
 * @FilePath: \sql\mychatroom.cpp
 */
/*
 * @Author: FengYanBin
 * @Date: 2021-08-05 15:00:11
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-14 11:49:03
 * @Description: file content
 * @FilePath: \sql\mychatroom.cpp
 */
#include "mychatroom.h"
#include "ui_mychatroom.h"

MyChatRoom::MyChatRoom(QWidget *parent) : CustomMoveWidget(parent),
                                          ui(new Ui::MyChatRoom),
                                          m_user_id(0), m_room_num(0), m_server_ip(kServerIp), m_server_port(kServerPort), m_audioRead(nullptr),
                                          m_pre_room_id(0), is_first_open_video(true), is_first_join(true)
{
    ui->setupUi(this);
    ui->pb_title->setFocusPolicy(Qt::NoFocus);
    ui->le_search->setPlaceholderText("请输入6位房间号id！");
    ui->lb_name->setText(this->m_user_name);

    //tcp连接
    m_tcp_client = new QMyTcpClient();
    m_tcp_client->InitNetWork(const_cast<char *>(m_server_ip.toStdString().c_str()), m_server_port);
    connect(m_tcp_client, SIGNAL(SIG_ReadyData(char *, int)), this, SLOT(SLOT_dealClientData(char *, int)));

    //登录界面显示
    m_login = new LOGIN();
    m_login->show();
    m_register = new registerWin();

    //客户端处理登录和注册请求
    connect(m_login, SIGNAL(SIG_loginSubmit(QString, QString)), this, SLOT(SLOT_loginSubmit(QString, QString)));
    connect(m_login, SIGNAL(SIG_JumpToRegisterInterface()), this, SLOT(DealJumpToRegisterInterface()));
    connect(m_login, SIGNAL(SIG_SkipLogin()), this, SLOT(SLOT_SkipLogin()));
    connect(m_register, SIGNAL(SIG_registerSubmit(QString, QString)), this, SLOT(SLOT_registerSubmit(QString, QString)));
    connect(m_register, SIGNAL(SIG_backToLoginInterface()), this, SLOT(SLOT_backToLoginInterface()));

    //客户端处理房间内的请求
    m_roomdialog = new RoomDialog();
    InitRoomDialogUi();
    InitRoomListUi();

    //房间页面
    connect(m_roomdialog, SIGNAL(SIG_openAudio()), this, SLOT(SLOT_openAudio()));
    connect(m_roomdialog, SIGNAL(SIG_closeAudio()), this, SLOT(SLOT_closeAudio()));
    connect(m_roomdialog, SIGNAL(SIG_refreshUserList()), this, SLOT(SLOT_refreshUserList()));
    connect(m_roomdialog, SIGNAL(SIG_quitRoomSubmit()), this, SLOT(SLOT_quitRoomSubmit()));
    connect(m_roomdialog, SIGNAL(SIG_unMute()), this, SLOT(SLOT_UnMute()));
    connect(m_roomdialog, SIGNAL(SIG_setAudio()), this, SLOT(SLOT_AudioSetting()));
    connect(m_user_list_widget, SIGNAL(SIG_muteUser(int)), this, SLOT(SLOT_MuteOneUser(int)));
    connect(m_user_list_widget, SIGNAL(SIG_closeVolumn(QString)), this, SLOT(SLOT_closeVolumn(QString)));
    connect(m_user_list_widget, SIGNAL(SIG_unBlock(QString)), this, SLOT(SLOT_unBlock(QString)));
    connect(m_user_list_widget, SIGNAL(SIG_transferUser(int)), this, SLOT(SLOT_transferOneUser(int)));
    connect(m_user_list_widget, SIGNAL(SIG_kick_out_ofUser(int)), this, SLOT(SLOT_kick_out_ofOneUser(int)));
    connect(m_user_list_widget, SIGNAL(SIG_adjustVolumnUser(int)), this, SLOT(SLOT_AdjustUserVolume(int)));

    m_audio_device = new AudioTest();


    //客户端处理加入房间和创建房间请求
    connect(m_room_widget, SIGNAL(SIG_joinRoom(int)), SLOT(SLOT_joinRoomSubmit(int)));
    connect(this, SIGNAL(SIG_joinRoomSubmit(int)), SLOT(SLOT_joinRoomSubmit(int)));
    connect(this, SIGNAL(SIG_createRoomSubmit(int)), this, SLOT(SLOT_createRoomSubmit(int)));
    connect(this, SIGNAL(SIG_RefreshRoomList()), this, SLOT(SLOT_RefreshRoomList()));

    connect(m_audio_device, SIGNAL(SIG_devicedChanged(QList<QAudioDeviceInfo>)), this, SLOT(SLOT_devicedChanged(QList<QAudioDeviceInfo>)));

    //定时器心跳检测
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(HeartDetect()));
    timer->start(50000);
}

void MyChatRoom::InitRoomDialogUi()
{

    m_user_list_widget = new UserListWidget(m_roomdialog);
    QPoint globalPos = m_roomdialog->mapToGlobal(QPoint(0, 0));
    m_user_list_widget->move(globalPos.x() + 480, globalPos.y() + 88);
    m_user_list_widget->setIconSize(QSize(50, 50));  //设置item图标大小
    m_user_list_widget->setFocusPolicy(Qt::NoFocus); //这样可禁用tab键和上下方向键并且除去复选框
    m_user_list_widget->setFixedSize(300, 400);
    m_user_list_widget->setFont(QFont("宋体", 10, QFont::DemiBold));
    m_user_list_widget->setStyleSheet("*{outline:0px;}");
    m_user_list_widget->setStyleSheet("QListWidget{background:rgb(245, 245, 247); border:0px; margin:0px 0px 0px 0px;}");
    m_user_list_widget->setStyleSheet("QListWidget::Item{height:40px; border:0px; padding-left:14px; color:rgba(200, 40, 40, 255);}");
    m_user_list_widget->setStyleSheet("QListWidget::Item:hover{color:rgba(40, 40, 200, 255);}");
    m_user_list_widget->setStyleSheet("QListWidget::Item:selected{background:rgb(230, 231, 234); color:rgba(40, 40, 200, 255); border-left:4px solid rgb(180, 0, 0);}");
    m_user_list_widget->setStyleSheet("QListWidget::Item:selected:active{background:rgb(230, 231, 234); color:rgba(40, 40, 200, 255); border-left:4px solid rgb(180, 0, 0);}");
}

void MyChatRoom::InitRoomListUi()
{
    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    m_room_widget = new RoomListWidget(this);
    m_room_widget->move(globalPos.x(), globalPos.y() + 240);
    m_room_widget->setIconSize(QSize(50, 50));  //设置item图标大小
    m_room_widget->setFocusPolicy(Qt::NoFocus); //这样可禁用tab键和上下方向键并且除去复选框
    m_room_widget->setFixedSize(301, 349);
    m_room_widget->setFont(QFont("宋体", 10, QFont::DemiBold));
    m_room_widget->setStyleSheet("*{outline:0px;}");
    m_room_widget->setStyleSheet("QListWidget{background:rgb(245, 245, 247); border:0px; margin:0px 0px 0px 0px;}");
    m_room_widget->setStyleSheet("QListWidget::Item{height:40px; border:0px; padding-left:14px; color:rgba(200, 40, 40, 255);}");
    m_room_widget->setStyleSheet("QListWidget::Item:hover{color:rgba(40, 40, 200, 255);}");
    m_room_widget->setStyleSheet("QListWidget::Item:selected{background:rgb(230, 231, 234); color:rgba(40, 40, 200, 255); border-left:4px solid rgb(180, 0, 0);}");
    m_room_widget->setStyleSheet("QListWidget::Item:selected:active{background:rgb(230, 231, 234); color:rgba(40, 40, 200, 255); border-left:4px solid rgb(180, 0, 0);}");
}

void MyChatRoom::clearQuitRoomInfo(QString name)
{
    this->m_room_owener_id = 0;
    this->m_room_num = 0;
    //用户列表清空
    this->m_user_list_widget->clear();
    //退出房间后音频也关闭
    if (m_audioRead)
    {
//        m_audioRead->PauseAudio();
        delete m_audioRead;
        m_audioRead = nullptr;
    }
    m_roomdialog->setPb_openAudioText();

    //音量界面也关闭
    if (m_userWidegets.find(name) != m_userWidegets.end()) {
        m_roomdialog->removeUserWidget(m_userWidegets[name]);
        auto it = m_userWidegets.find(name);
        m_userWidegets.erase(it);
    }

    if (!m_roomdialog->isHidden())
    {
        m_roomdialog->close();
        this->show();
    }
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

void MyChatRoom::HeartDetect()
{
    //qDebug() << "heart!" << endl;
    char *buf = new char[8];
    char *tmp = buf;

    *(int *)tmp = kPackHeartDetect;
    tmp += sizeof(int);

    *(int *)tmp = m_user_id;

    m_tcp_client->SendData(buf, sizeof(buf));
    delete[] buf;
}

void MyChatRoom::SLOT_loginSubmit(QString name, QString passwd)
{
    //发送给服务器
    qDebug() << "发给服务器登录请求！" << endl;
    StructLoginRequest rq;
    std::string strTmp = name.toStdString();
    char *buf = const_cast<char *>(strTmp.c_str());
    this->m_user_name = name;
    strcpy_s(rq.m_user_name, buf);

    //QByteArray ba = GetMD5(passwd);
    QByteArray ba = passwd.toUtf8();
    memcpy(rq.m_user_passwd, ba.data(), static_cast<size_t>(ba.length()));

    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_registerSubmit(QString name, QString passwd)
{
    qDebug() << "registerSubmit!" << endl;
    //发送给服务器
    StructRegisterRequest rq;
    std::string strTmp = name.toStdString();
    char *buf = const_cast<char *>(strTmp.c_str());
    strcpy_s(rq.m_user_name, buf);

    //QByteArray ba = GetMD5(passwd);
    QByteArray ba = passwd.toUtf8();
    memcpy(rq.m_user_passwd, ba.data(), static_cast<size_t>(ba.length()));

    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
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
    char *buf = const_cast<char *>(strTmp.c_str());
    strcpy_s(rq.m_user_name, buf);
    this->m_user_name = "davy";

    QString passwd = "123";
    //QByteArray ba = GetMD5(passwd);
    QByteArray ba = passwd.toUtf8();
    memcpy(rq.m_user_passwd, ba.data(), static_cast<size_t>(ba.length()));

    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
    m_login->close();
    this->show();
}

void MyChatRoom::SLOT_createRoomSubmit(int user_id)
{
    qDebug() << "createRoomSubmit!" << endl;
    //发送给服务器
    StructCreateRoomRequest rq;
    rq.m_user_id = user_id;

    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_joinRoomSubmit(int room_num)
{
    qDebug() << "joinRoomSubmit!" << endl;
    //发送给服务器
    StructJoinRoomRequest rq;
    rq.m_user_id = this->m_user_id;
    rq.m_room_id = room_num;

    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));

    //udp连接，进入房间后选择打开音频
}

void MyChatRoom::SLOT_quitRoomSubmit()
{
    qDebug() << "user_id=!" << m_user_id << " "
             << "QuitRoomSubmit!" << endl;
    //发送给服务器
    StructQuitRoomRequest rq;
    rq.m_user_id = m_user_id;
    rq.m_room_id = m_room_num;

    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));

    //退出房间，暂停声音
    if (m_audioRead)
    {
        m_audioRead->PauseAudio();
    }

    //销毁声音播放
    //    for (auto it = m_mapIDToAudioWrite.begin(); it != m_mapIDToAudioWrite.end();)
    //    {
    //        delete *it;
    //        //清空map
    //        it = m_mapIDToAudioWrite.erase(it);
    //    }
    m_room_num = 0;
    m_roomdialog->close();
    this->show();
}

void MyChatRoom::SLOT_RefreshRoomList()
{
    qDebug() << "SLOT_RefreshRoomList" << endl;
    StructRoomListRefreshHeaderRequest rq;
    rq.m_user_id = this->m_user_id;
    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
}

//打开音频
void MyChatRoom::SLOT_openAudio()
{
    if (m_audioRead)
    {
        if (is_first_open_video) {
            QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
            m_device_info = info;
            m_audioRead->ResumeAudio(info);
        } else {
            m_audioRead->ResumeAudio(m_device_info);
        }


    }
}

//关闭音频
void MyChatRoom::SLOT_closeAudio()
{
    if (m_audioRead)
    {
        m_audioRead->PauseAudio();
    }
}

void MyChatRoom::SLOT_sendAudioData(QByteArray ba)
{
    ///音频数据帧
    /// 成员描述
    /// int type;
    /// int userId;
    /// int roomId;
    /// QByteArray audioFrame;
    /// q
    // qDebug() << "发送音频数据！" << endl;
    int nlen = ba.size() + 12;
    char *buf = new char[nlen];
    char *tmp = buf;

    *(int *)tmp = kPackAudioQequest;
    tmp += sizeof(int);

    *(int *)tmp = m_user_id;
    tmp += sizeof(int);

    *(int *)tmp = m_room_num;
    tmp += sizeof(int);

    memcpy(tmp, ba.data(), ba.size());

    m_tcp_client->SendData(buf, nlen);
    delete[] buf;
}

void MyChatRoom::SLOT_UnMute()
{
    qDebug() << "SLOT_UnMute" << endl;
    if (this->is_muted == false)
    {
        QMessageBox::information(this, "提示", "您当前没有被静音！");
        return;
    }
    StructUnMuteRequest rq;
    rq.m_user_id = this->m_user_id;
    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_MuteOneUser(int mute_user_id)
{
    qDebug() << "SLOT_MuteOneUser" << endl;
    if (mute_user_id == this->m_room_owener_id)
    {
        QMessageBox::information(this, "提示", "该用户是房主，无法静音！");
        return;
    }
    else if (mute_user_id == this->m_user_id)
    {
        QMessageBox::information(this, "提示", "无需静音自己，请关闭自己的声音即可！");
        return;
    }
    StructMuteUserRequest rq;
    rq.m_user_id = this->m_user_id;
    rq.mute_user_id = mute_user_id;
    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_transferOneUser(int transfer_user_id)
{
    qDebug() << "SLOT_transferOneUser" << endl;
    if (this->m_user_id != this->m_room_owener_id)
    {
        QMessageBox::information(this, "提示", "您不是房主，没有该权限");
        return;
    }
    else if (this->m_user_id == transfer_user_id)
    {
        QMessageBox::information(this, "提示", "您已经是房主");
        return;
    }
    StructTransferUserRequest rq;
    rq.m_user_id = this->m_user_id;
    rq.transfer_user_id = transfer_user_id;

    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_kick_out_ofOneUser(int kick_user_id)
{
    qDebug() << "SLOT_kick_out_ofOneUser" << endl;
    if (this->m_user_id != this->m_room_owener_id)
    {
        QMessageBox::information(this, "提示", "您不是房主，没有该权限");
        return;
    }
    else if (this->m_user_id == kick_user_id)
    {
        QMessageBox::information(this, "提示", "不能踢出自己，请选择退出房间！");
        return;
    }
    StructKickOutOfUserRequest rq;
    rq.m_user_id = this->m_user_id;
    rq.kick_user_id = kick_user_id;

    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_AdjustUserVolume(int adjust_user_id)
{
}

void MyChatRoom::SLOT_closeVolumn(QString name)
{
    std::string str = name.toStdString();
    m_audioRead->insertMuteUser(str);
    QMessageBox::information(this, "提示", QString("该用户已经被您屏蔽！"));
}

void MyChatRoom::SLOT_unBlock(QString name)
{
    std::string str = name.toStdString();
    m_audioRead->delMuteUser(str);
    QMessageBox::information(this, "提示", QString("解除对该用户的屏蔽！"));
}

void MyChatRoom::SLOT_AudioSetting()
{
    //TODO:弹出设置界面
    m_audio_device->show();
}

void MyChatRoom::SLOT_devicedChanged(QList<QAudioDeviceInfo> audio_list)
{
    QString device_name = m_audio_device->GetCurrentDevice();
    qDebug() << "当前设备名称：" << device_name << endl;
    QAudioDeviceInfo info;
    for (auto it = audio_list.begin(); it != audio_list.end(); ++it) {
        if ((*it).deviceName() == device_name)
            info = (*it);
    }
    m_device_info = info;
    if (m_audioRead)
        m_audioRead->ResumeAudio(info);
}

void MyChatRoom::SLOT_refreshUserList()
{
    StructRoomMemberRequset rq;
    rq.m_user_id = this->m_user_id;
    m_tcp_client->SendData(reinterpret_cast<char *>(&rq), sizeof(rq));
}

void MyChatRoom::SLOT_dealClientData(char *buf, int len)
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

    case kPackRoomMemberQesponse:
        DealRefreshUserList(buf, len);
        break;

    case kPackRoomMemberHeaderQequest:
        DealRefreshUserListHeader(buf, len);
        break;

    case kPackQuitRoomQesponse:
        DealQuitRoomResponse(buf, len);
        break;

    case kPackMuteOneUserQesponse:
        DealMuteOneUserResponse(buf, len);
        break;

    case kPackUnmuteQesponse:
        DealUnmuteQequest(buf, len);
        break;

    case kPackTransferUserQesponse:
        DealtransferOneUserResponse(buf, len);
        break;

    case kPackKickOutOfUserQesponse:
        Dealkick_out_ofOneUserResponse(buf, len);
        break;

    case kPackAdjustUserVolumeQesponse:;
        break;

    case kPackAudioQequest:
        //DealAudioData(buf, len);
        break;
    }
}

void MyChatRoom::DealLoginResponse(char *buf, int len)
{
    StructLoginRespose *rs = reinterpret_cast<StructLoginRespose *>(buf);
    switch (rs->m_login_result)
    {
    case kUserIdNotExist:
        QMessageBox::information(this->m_login, "提示", "用户不存在, 登录失败");
        break;
    case kPasswdError:
        QMessageBox::information(this->m_login, "提示", "密码错误, 登录失败");
        break;
    case kLoginSucess:
        this->m_login->close();
        this->show();
        this->ui->lb_name->setText(m_user_name);
        this->m_user_id = rs->m_user_id;
        g_userName = this->m_user_name;
        break;
    }
}

void MyChatRoom::DealRegisterResponse(char *buf, int len)
{
    StructRegisterRespose *rs = reinterpret_cast<StructRegisterRespose *>(buf);
    switch (rs->m_register_result)
    {
    case kUserIdExist:
        QMessageBox::information(this->m_login, "提示", "用户已存在, 注册失败");
        break;
    case kRegisterSucess:
        QMessageBox::information(this->m_login, "提示", "注册成功");
        break;
    }
}

void MyChatRoom::DealCreateRoomResponse(char *buf, int len)
{
    //解析
    StructCreateRoomRespose *rs = reinterpret_cast<StructCreateRoomRespose *>(buf);
    if (rs->m_create_room_result) //结果成功
    {                             //设置房间标志
        this->m_room_num = rs->m_room_id;
        this->m_room_owener_id = rs->m_room_owner_id;
        this->m_room_owner_name = rs->m_room_owner_name;

        //房间显示以及设置
        this->hide();
        m_roomdialog->setInfo(rs->m_room_id, this->m_user_name, this->m_room_owner_name);
        m_roomdialog->show();
        m_roomdialog->addInputWidget(m_level);

        m_user_list.insert(this->m_user_name.toStdString());

        //创建音频采集,TODO，this对不对，还是m_roomDialog。
        m_audioRead = new AudioCollector();

        QObject::connect(m_audioRead, &AudioCollector::sig_collectorVolumeReady, [this](double volume)
                         { m_level.setLevel(volume); });

        QObject::connect(m_audioRead, &AudioCollector::sig_userVolumeReady, [this](QString name, double volume)
                         {
                             if (m_userWidegets.find(name) != m_userWidegets.end())
                                 this->m_userWidegets[name]->setVol(volume);
                         });
        m_audioRead->Init(m_room_num);
    }
}

void MyChatRoom::DealJoinRoomResponse(char *buf, int len)
{
    //解析
    StructJoinRoomResponse *rs = reinterpret_cast<StructJoinRoomResponse *>(buf);
    if (rs->m_join_id == this->m_user_id) {
        if (rs->m_join_room_result) //结果成功
        {                           //设置房间标志
            this->m_room_num = rs->m_room_id;
            this->m_room_owener_id = rs->m_room_owner_id;
            this->m_room_owner_name = rs->m_room_owner_name;

            qDebug() << "进入房间:" << len << endl;
            //房间显示以及设置
            this->hide();
            m_roomdialog->setInfo(rs->m_room_id, this->m_user_name, this->m_room_owner_name);
            m_roomdialog->show();
            m_roomdialog->addInputWidget(m_level);
            m_user_list.insert(this->m_user_name.toStdString());
            //创建音频采集
            m_audioRead = new AudioCollector();

            QObject::connect(m_audioRead, &AudioCollector::sig_collectorVolumeReady, [this](double volume)
                             { m_level.setLevel(volume); });

            QObject::connect(m_audioRead, &AudioCollector::sig_userVolumeReady, [this](QString name, double volume)
                             {
                                 if (m_userWidegets.find(name) != m_userWidegets.end())
                                     this->m_userWidegets[name]->setVol(volume);
                             });
            m_audioRead->Init(m_room_num);
        }
    }
    else {
        m_user_list.insert(rs->m_join_name);
    }

}

void MyChatRoom::DealRefreshRoomListHeader(char *buf, int len)
{
    qDebug() << "DealRefreshRoomListHeader!" << endl;
    m_room_widget->clear();
}

//TODO：优化，现在每次都要全部刷新，只改发生变化的即可
void MyChatRoom::DealRefreshRoomList(char *buf, int len)
{
    qDebug() << "刷新房间列表！" << endl;
    StructRoomListResponse *rs = reinterpret_cast<StructRoomListResponse *>(buf);

    //显示房间列表到客户端，要求显示房间号，房主。房主就是用户列表的头部。
    int num = rs->m_room_num;
    QString name = rs->m_owner_name;
    QString text = QString("    '%1'                            '%2'").arg(num).arg(name);
    RoomListWidgetItem *item = new RoomListWidgetItem(m_room_widget);
    item->setSizeHint(QSize(50, 50));
    item->setText(text);
}

void MyChatRoom::DealRefreshUserListHeader(char *buf, int len)
{
    qDebug() << "清空用户列表！" << endl;
    //清空用户列表
    m_user_list_widget->clear();
    //清空用户音量显示
    for (auto it = m_userWidegets.begin(); it != m_userWidegets.end(); ++it) {
        m_roomdialog->removeUserWidget(it.value());

    }
    m_userWidegets.clear();
    qDebug() << "清空了！"  << endl;
}

void MyChatRoom::DealRefreshUserList(char *buf, int len)
{
    qDebug() << "更新用户列表！" << endl;
    StructRoomMemberResponse *rs = reinterpret_cast<StructRoomMemberResponse *>(buf);
    if (m_user_list.find(rs->m_sz_user) == m_user_list.end())
        m_user_list.insert(this->m_user_name.toStdString());
    int user_id = rs->m_user_id;
    QString name = rs->m_sz_user;
    QString text = QString("    '%1'         '%2'").arg(user_id).arg(name);

    UserListWidgetItem *item = new UserListWidgetItem(m_user_list_widget);
    item->setSizeHint(QSize(50, 50));
    item->setText(text);

    //声音图像显示
    //逻辑：udp接收到服务器数据SIG_oneMsgReady，AudioSynthesizer::onOneFrameIn处理，进行合成，并当i=10也就是收到10个数据后会发送sig_userListReady信号。
    //TODO:他设置了心跳检测，将用户说话声音大于3s的才加入队列。我一开始是静音的，那么需要点两次才能让队列里有内容。

    QObject::connect(m_audioRead, &AudioCollector::sig_userListReady, [this](QList<QString> list)
                     {
                         //it代表用户名
                         for (auto &item : list)
                         {
                             //QString item = QString::fromStdString(it);
                             if (m_userWidegets.find(item) == m_userWidegets.end())
                             {
                                 UserWidget *newOne = new UserWidget(item, m_audioRead, m_room_widget);
                                 connect(newOne, &UserWidget::sig_sliderMoved, [this, item](int val)
                                         { this->m_audioRead->setUserVolume(item, val); });

                                 m_userWidegets[item] = newOne;
                                 m_roomdialog->addUserWidget(newOne);
                             }
                         }
                     });
}

//这里有个逻辑问题，其实服务器应该返回给数据才关闭。但是客户端自己关闭就够了。
void MyChatRoom::DealQuitRoomResponse(char *buf, int len)
{
    qDebug() << "退出房间！" << endl;
    //解析包
    StructQuitRoomResponse *rs = reinterpret_cast<StructQuitRoomResponse *>(buf);

    //如果回复的用户id是自己，那么关闭房间框
    if (rs->m_user_id == this->m_user_id)
    {
        clearQuitRoomInfo(rs->szUserName);
        m_user_list.erase(this->m_user_name.toStdString());

    }
}

void MyChatRoom::DealMuteOneUserResponse(char *buf, int len)
{
    qDebug() << "静音用户" << endl;
    //解析包
    StructMuteUserResponse *rs = reinterpret_cast<StructMuteUserResponse *>(buf);

    if (rs->mute_user_id == this->m_user_id)
    {
        is_muted = true;
        QMessageBox::information(this, "提示", "您已经被静音！");
        m_audioRead->MuteUser();
    }
    else
    {
        QMessageBox::information(this, "提示", QString("用户：%1已经被静音").arg(rs->muteUserName));
    }
}

void MyChatRoom::DealtransferOneUserResponse(char *buf, int len)
{
    qDebug() << "接收转让房主信息！" << endl;
    //解析包
    StructTransferUserResponse *rs = reinterpret_cast<StructTransferUserResponse *>(buf);
    //如果被转让为房主
    if (this->m_user_id == rs->transfer_user_id)
    {
        QMessageBox::information(this, "提示", "您被转让成为房主！");
        this->m_room_owener_id = rs->transfer_user_id;

        m_roomdialog->setRoomOwner(rs->transferUserName);
    }
    else if (this->m_user_id == rs->m_user_id)
    { //提示原房主转让成功
        QMessageBox::information(this, "提示", "转让成功！");
        this->m_room_owener_id = rs->transfer_user_id;
        m_roomdialog->setRoomOwner(rs->transferUserName);
    }
    else
    {
        QMessageBox::information(this, "提示", "当前房主已经更改，位于列表第一位！");
        this->m_room_owener_id = rs->transfer_user_id;
        m_roomdialog->setRoomOwner(rs->transferUserName);
    }
}

void MyChatRoom::Dealkick_out_ofOneUserResponse(char *buf, int len)
{
    qDebug() << "接收踢出信息！" << endl;
    //解析包
    StructKickOutOfUserResponse *rs = reinterpret_cast<StructKickOutOfUserResponse *>(buf);
    //如果自己是被踢的
    if (this->m_user_id == rs->kick_user_id)
    {
        QMessageBox::information(this, "提示", "您已被踢出房间！");
        m_user_list.erase(this->m_user_name.toStdString());
        clearQuitRoomInfo(rs->szUserName);
    }
    else if (this->m_user_id == rs->m_user_id)
    { //提示房主踢人成功
        QMessageBox::information(this, "提示", "踢出成功！");
    }
    else //提示用户有人被踢出
    {
        QMessageBox::information(this, "提示", "xx用户被移出房间！");
    }
}

void MyChatRoom::DealAdjustUserVolumeResponse(char *buf, int len)
{
}

void MyChatRoom::DealUnmuteQequest(char *buf, int len)
{
    qDebug() << "接收解除静音回复！" << endl;

    //解析包
    StructUnMuteResponse *rs = reinterpret_cast<StructUnMuteResponse *>(buf);
    //如果是自己发出的
    if (this->m_user_id == rs->m_user_id)
    {
        QMessageBox::information(this, "提示", "您已解除静音！");
        this->is_muted = false;
        m_audioRead->UnMuteUser();
    }
    else
    {
        QMessageBox::information(this, "提示", QString("用户：%1解除静音").arg(rs->szUserName));
    }
}

void MyChatRoom::DealForceQuitRoom(char *buf, int len)
{
}

void MyChatRoom::DealClientQuitResponse(char *buf, int len)
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
    m_audio_device->show();
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

void MyChatRoom::on_ButtonRefreshRoomList_clicked()
{
    emit SIG_RefreshRoomList();
}