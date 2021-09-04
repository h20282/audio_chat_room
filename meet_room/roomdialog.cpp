#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#include "roomdialog.h"
#include "ui_roomdialog.h"
#include <QMessageBox>

#include "log/log.h"

RoomDialog::RoomDialog(QWidget *parent) : CustomMoveDialog(parent),
                                          ui(new Ui::RoomDialog), m_roomid(123456)
{
    ui->setupUi(this);
    this->reCheckInputDevice();
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

void RoomDialog::reCheckInputDevice(){
    LOG_INFO("call reCheckInputDevice");
    ui->audioInputComboBox->clear();

    QAudioDeviceInfo info;
    for (auto curr_info : QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        LOG_INFO("{} added", curr_info.deviceName().toStdString());
        ui->audioInputComboBox->addItem(curr_info.deviceName());
    }
}

//最小化
void RoomDialog::on_pb_min_clicked()
{
    this->showMinimized();
}

//退出房间
void RoomDialog::on_pb_quitRoom_clicked()
{
    if (0 == m_roomid)
    {
        QMessageBox::about(this, "提示", "先加入房间");
        return;
    }
    if (QMessageBox::question(this, "退出房间", "确认退出房间?") == QMessageBox::No)
    {
        return;
    }
    //发信号
    emit SIG_quitRoomSubmit();
}

//关闭
void RoomDialog::on_pb_close_clicked()
{
    on_pb_quitRoom_clicked();
}

//清空控件
void RoomDialog::clear()
{
    ui->pb_openAudio->setText("打开音频");
    ui->lb_tittle->setText("房间号:---");
    ui->lb_name->setText("用户名:---");
    ui->lb_owner->setText("房主名：---");
    m_roomid = 0;
}

//打开音频
void RoomDialog::on_pb_openAudio_clicked()
{
    if (0 == m_roomid)
    {
        QMessageBox::about(this, "提示", "先加入房间");
        return;
    }
    if (ui->pb_openAudio->text() == "打开音频")
    {
        emit SIG_openAudio();
        ui->pb_openAudio->setText("关闭音频");
    }
    else
    {
        emit SIG_closeAudio();
        ui->pb_openAudio->setText("打开音频");
    }
}


Ui::RoomDialog *RoomDialog::getUi() const
{
    return ui;
}

//设置房间id
void RoomDialog::setRoomid(const int &roomid)
{
    m_roomid = roomid;
    ui->lb_tittle->setText(QString("房间号:%1").arg(m_roomid));
}
//设置用户id
void RoomDialog::setUserid(const QString &username)
{
    this->m_user_name = username;
    ui->lb_name->setText(QString("用户名:%1").arg(username));
}

void RoomDialog::setRoomOwner(const QString &owner)
{
    ui->lb_owner->setText(QString("房主名:%1").arg(owner));
}

void RoomDialog::setInfo(const int &roomid, const QString &username, const QString &owner)
{
    m_roomid = roomid;
    ui->lb_tittle->setText(QString("房间号:%1").arg(m_roomid));
    this->m_user_name = username;
    ui->lb_name->setText(QString("用户名:%1").arg(username));
    ui->lb_owner->setText(QString("房主名:%1").arg(owner));
}

void RoomDialog::addInputWidget(AudioLevel& level)
{
    ui->inputVolumeLayout->addWidget(&level);
}

void RoomDialog::addUserWidget(UserWidget *user_widget)
{
    ui->userListLayout->addWidget(user_widget);
}

void RoomDialog::removeUserWidget(UserWidget *user_widget)
{
    ui->userListLayout->removeWidget(user_widget);
    user_widget->setParent(nullptr);
}

void RoomDialog::setPb_openAudioText()
{
    ui->pb_openAudio->setText("关闭音频");
}

QVBoxLayout* RoomDialog::getuserListLayout()
{
    return ui->userListLayout;
}
//最大化
void RoomDialog::on_pb_max_clicked()
{
    this->slot_showMax();
}

void RoomDialog::on_pb_unmute_clicked()
{
    emit SIG_unMute();
}


void RoomDialog::on_pb_userlist_clicked()
{
    emit SIG_refreshUserList();
}


void RoomDialog::on_reCheckBtn_clicked()
{
    this->reCheckInputDevice();
}

void RoomDialog::on_audioInputComboBox_activated(const QString &name)
{
    for (auto curr_info : QAudioDeviceInfo::availableDevices(QAudio::AudioInput)) {
        if ( curr_info.deviceName() == name ) {
            emit SIG_setInputDevice(curr_info);
            QMessageBox::information(this, "设置成功", "已设置为：" + name);
            return;
        }
    }
    QMessageBox::information(this, "设置失败", "未找到输入设备：" + name + "，请点击“检测”");
}
