/*
 * @Author: FengYanBin
 * @Date: 2021-08-04 14:49:47
 * @LastEditTime: 2021-08-04 20:02:16
 * @LastEditors: FengYanBin
 * @Description: In User Ssettings Editss
 * @FilePath: \sql\main.cpp
 */
#include "mainwindow.h"
#include "./login_register/login.h"
#include "./login_register/common.h"
#include "./net_api/udp_net.h"
#include "mychatroom.h"

#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QStyleFactory> //修改风格1步
#include <QSqlDatabase>
#include <QMessageBox>

QSqlDatabase db;
static bool createconnection()
{
    db = QSqlDatabase::addDatabase("QODBC");
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setDatabaseName("user");
    db.setUserName("root");
    db.setPassword("123"); //设置数据库连接账号的密码
    bool ok = db.open();
    if (ok)
    {
        qDebug() << "数据库连接成功";
        return 1;
    }
    else
    {
        QMessageBox::information(NULL, "提示", "连接数据库失败!", QMessageBox::Yes);
        return 0;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QProcess process;
    process.start("D:/Program files/mysql-8.0.26-winx64/bin/mysqld.exe");
    if (!createconnection())
        return true;

    qDebug() << "test!" << endl;
    UdpNet* udpConnect = new UdpNet();
    QByteArray data;
    data.resize(1);
    data[0] = 0xfE;



    QFile file(":/qss/default.css");
    file.open(QIODevice::ReadOnly);
    qApp->setStyleSheet( file.readAll());

    //初始化，进入登录界面。
    MyChatRoom mychatroom;

    //MainWindow w;
    mychatroom.hide();
    udpConnect->SendData(data, "119.91.116.26");

    return a.exec();
}
