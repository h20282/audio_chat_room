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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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
    qDebug() << "udp连接" << endl;
    udpConnect->SendData(data, "119.91.116.26");

    return a.exec();
}
