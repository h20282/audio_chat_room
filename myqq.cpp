/*
 * @Author: FengYanBin
 * @Date: 2021-08-05 15:00:11
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-05 16:31:42
 * @Description: file content
 * @FilePath: \sql\myqq.cpp
 */
#include "myqq.h"
#include "ui_myqq.h"

MyQQ::MyQQ(QWidget *parent) : QWidget(parent),
                              ui(new Ui::MyQQ)
{
    ui->setupUi(this);
    this->setWindowTitle("多人语音聊天");
    ui->le_search->setPlaceholderText("请输入6位房间号id！");
    connect(this, SIGNAL(toEnterRoom(uint32_t)), SLOT(OnToEnterRoom()));
}

MyQQ::~MyQQ()
{
    delete ui;
}

void MyQQ::OnToEnterRoom()
{
    qDebug() << "进入房间号" << endl;
    roomdialog = new RoomDialog();
    roomdialog->show();
}

void MyQQ::on_pb_search_clicked()
{
    if (ui->le_search->text() == QString("123456"))
    {
        qDebug() << "房间号输入正确!" << endl;
        roomdialog = new RoomDialog();
        roomdialog->show();
    }
    else {
        QMessageBox::information(nullptr, "提示:","该房间不存在!", QMessageBox::Ok);
    }
}

void MyQQ::on_le_search_returnPressed()
{
    if (ui->le_search->text() == QString("123456"))
    {
        qDebug() << "房间号输入正确!" << endl;
        roomdialog = new RoomDialog();
        roomdialog->show();
    }
    else {
        QMessageBox::information(nullptr, "提示:","该房间不存在!", QMessageBox::Ok);
    }
}
