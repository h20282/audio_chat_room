#include "roomdialog.h"
#include "ui_roomdialog.h"
#include <QMessageBox>

RoomDialog::RoomDialog(QWidget *parent) : CustomMoveDialog(parent),
                                          ui(new Ui::RoomDialog), m_roomid(123456)
{
    ui->setupUi(this);
    //initUi();
}

RoomDialog::~RoomDialog()
{
    delete ui;
}

void RoomDialog::initUi()
{
    //    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
    //    listwidget = new UserListWidget(this);
    //    listwidget->move(globalPos.x() + 200, globalPos.y() + 80);
    //    listwidget->setIconSize(QSize(50, 50));  //设置item图标大小
    //    listwidget->setFocusPolicy(Qt::NoFocus);  //这样可禁用tab键和上下方向键并且除去复选框
    //    listwidget->setFixedHeight(400);
    //    listwidget->setFixedWidth(300);
    //    listwidget->setFont(QFont("宋体", 10, QFont::DemiBold));
    //    listwidget->setStyleSheet("*{outline:0px;}");
    //    listwidget->setStyleSheet("QListWidget{background:rgb(245, 245, 247); border:0px; margin:0px 0px 0px 0px;}");
    //    listwidget->setStyleSheet("QListWidget::Item{height:40px; border:0px; padding-left:14px; color:rgba(200, 40, 40, 255);}");
    //    listwidget->setStyleSheet("QListWidget::Item:hover{color:rgba(40, 40, 200, 255);}");
    //    listwidget->setStyleSheet("QListWidget::Item:selected{background:rgb(230, 231, 234); color:rgba(40, 40, 200, 255); border-left:4px solid rgb(180, 0, 0);}");
    //    listwidget->setStyleSheet("QListWidget::Item:selected:active{background:rgb(230, 231, 234); color:rgba(40, 40, 200, 255); border-left:4px solid rgb(180, 0, 0);}");

    //    UserListWidgetItem *item = new UserListWidgetItem(listwidget);
    //    item->setSizeHint(QSize(50, 50));
    //    item->setText("textasfafas");

    //    QVBoxLayout *layout = new QVBoxLayout(this);
    //    layout->setSpacing(0);
    //    layout->addWidget(listwidget);
    //    layout->setContentsMargins(0, 0, 0, 0);
    //    setLayout(layout);
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
    //    //清空控件
    //    clear();
    //    this->close();

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
    ui->pb_openVideo->setText("打开视频");
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

//打开视频
void RoomDialog::on_pb_openVideo_clicked()
{
    if (0 == m_roomid)
    {
        QMessageBox::about(this, "提示", "先加入房间");
        return;
    }

    if (ui->pb_openVideo->text() == "打开视频")
    {
        emit SIG_openVideo();
        ui->pb_openVideo->setText("关闭视频");
    }
    else
    {
        emit SIG_closeVideo();
        ui->pb_openVideo->setText("打开视频");
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
    ui->pb_openAudio->setText("打开音频");
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

//void RoomDialog::contextMenuEvent ( QContextMenuEvent * event )
//{
//    QMenu* popMenu = new QMenu(listwidget);
//    popMenu->addAction(new QAction("添加", this));
//    popMenu->addAction(new QAction("删除", this));
//    if(listwidget->itemAt(mapFromGlobal(QCursor::pos())) != NULL) //如果有item则添加"修改"菜单 [1]*
//    {
//        popMenu->addAction(new QAction("修改", this));
//    }

//    popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
//}

void RoomDialog::on_pb_unmute_clicked()
{
    emit SIG_unMute();
}

void RoomDialog::on_pb_setting_clicked()
{
    emit SIG_setAudio();
}

void RoomDialog::on_pb_userlist_clicked()
{
    emit SIG_refreshUserList();
}
