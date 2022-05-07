/*
 * @Author: FengYanBin
 * @Date: 2021-08-04 14:49:47
 * @LastEditTime: 2021-08-04 20:02:16
 * @LastEditors: FengYanBin
 * @Description: In User Ssettings Editss
 * @FilePath: \sql\main.cpp
 */

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QProcess>
#include <QStyleFactory>  //修改风格1步

#include "spdlog/spdlog.h"

#include "./login_register/common.h"
#include "./login_register/login.h"
#include "mainwindow.h"
#include "mychatroom.h"

int main(int argc, char *argv[]) {
    {
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%l][%s,%!]:%v");
        spdlog::set_level(spdlog::level::level_enum::debug);
    }

    QApplication a(argc, argv);

    QFile file(":/qss/default.css");
    file.open(QIODevice::ReadOnly);
    qApp->setStyleSheet(file.readAll());

    //初始化，进入登录界面。
    MyChatRoom mychatroom;
    mychatroom.hide();

    return a.exec();
}
