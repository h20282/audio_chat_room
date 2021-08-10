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

    m_login = new LOGIN();
    m_login->show();

    //connect(m_login, &LOGIN::SIG_loginSubmit, this, &MyChatRoom::SLOT_loginSubmit);
    connect(m_login, SIGNAL(SIG_loginSubmit(QString, QString)), this, SLOT(SLOT_loginSubmit(QString, QString)));
    connect(m_login, SIGNAL(SIG_registerReq(QString, QString)), this, SLOT(SLOT_registerSubmit(QString, QString)));

    m_tcp_client = new QMyTcpClient();
    m_tcp_client->InitNetWork(const_cast<char *>(m_server_ip.toStdString().c_str()), m_server_port);

    ui->pb_title->setFocusPolicy(Qt::NoFocus);
    ui->le_search->setPlaceholderText("请输入6位房间号id！");

    connect(m_tcp_client, SIGNAL(SIG_ReadyData(char *, int)), this, SLOT(SLOT_dealClientData(char *, int)));

    m_roomdialog = new RoomDialog();
    connect(m_roomdialog, SIGNAL(SIG_openAudio()), this, SLOT(SLOT_openAudio()));
    connect(m_roomdialog, SIGNAL(SIG_closeAudio()), this, SLOT(SLOT_closeAudio()));
    connect(m_roomdialog, SIGNAL(SIG_quitRoom(int)), this, SLOT(SLOT_quitRoomSubmit(int)));

    connect(this, SIGNAL(SIG_joinRoomSubmit(int)), SLOT(SLOT_joinRoomSubmit(int)));
    connect(this, SIGNAL(SIG_createRoomSubmit()), this, SLOT(SLOT_createRoomSubmit()));
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
    m_login->close();
    this->show();
    this->setWindowTitle("多人语音聊天");
    this->ui->lb_name->setText(m_user_name);
}

void MyChatRoom::SLOT_registerSubmit(QString name, QString passwd)
{
}

void MyChatRoom::SLOT_createRoomSubmit()
{
    qDebug() << "sss" << endl;
    this->hide();
    m_roomdialog->show();
}

void MyChatRoom::SLOT_joinRoomSubmit(int room_num)
{
    this->hide();
    m_roomdialog->show();
}

void MyChatRoom::SLOT_quitRoomSubmit(int room_num)
{
    //发送给服务端关闭请求，这里暂时只是让客户端退出。
    m_roomdialog->close();
    this->show();
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
            this->m_login->hide();
            this->show();
            this->ui->lb_name->setText(m_user_name);
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


        //房间显示以及设置
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

        qDebug() << "进入房间:" << len << endl;
        //房间显示以及设置
        m_roomdialog->setRoomid(rs->m_room_id);
        m_roomdialog->show();

//        //创建音频采集
//        m_pAudioRead = new Audio_Read;
//        connect(m_pAudioRead, SIGNAL(sig_net_tx_frame(QByteArray)) ,
//                this , SLOT(slot_sendAudioFrame(QByteArray)) );
    }
}

void MyChatRoom::DealQuitRoomResponse(char* buf, int len)
{
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
    m_roomdialog->close();
    this->show();
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

void MyChatRoom::on_pb_search_clicked()
{
    int room_num = ui->le_search->text().toInt();
    emit SIG_joinRoomSubmit(room_num);

    //这些是服务器判断的
    if (ui->le_search->text() == QString("123456"))
    {
        qDebug() << "房间号输入正确!" << endl;
        m_roomdialog->show();
    }
    else
    {
        QMessageBox::information(nullptr, "提示:", "该房间不存在!", QMessageBox::Ok);
    }
}

void MyChatRoom::on_le_search_returnPressed()
{
    int room_num = ui->le_search->text().toInt();
    emit SIG_joinRoomSubmit(room_num);

    //这些是服务器判断的
    if (ui->le_search->text() == QString("123456"))
    {
        qDebug() << "房间号输入正确!" << endl;
        m_roomdialog->show();
    }
    else
    {
        QMessageBox::information(nullptr, "提示:", "该房间不存在!", QMessageBox::Ok);
    }
}

void MyChatRoom::on_ButtonCreateRoom_clicked()
{
    emit SIG_createRoomSubmit();
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
