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

#include <QCoreApplication>
#include <QApplication>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QStyleFactory> //修改风格1步
//#include <QSqlDatabase>
#include <QMessageBox>

extern "C"{

#include <libavcodec/avcodec.h>

#include <libavformat/avformat.h>

#include <libswscale/swscale.h>

#include <libavdevice/avdevice.h>

#include <libavformat/version.h>

#include <libavutil/time.h>

#include <libavutil/mathematics.h>

}


QString g_userName;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


//    qDebug()<<QString(avcodec_configuration());
//    qDebug()<<"version:"<<QString::number(avcodec_version());


    QFile file(":/qss/default.css");
    file.open(QIODevice::ReadOnly);
    qApp->setStyleSheet( file.readAll());

    //初始化，进入登录界面。
    MyChatRoom mychatroom;
    mychatroom.hide();

    return a.exec();
}
