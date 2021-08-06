/*
 * @Author: FengYanBin
 * @Date: 2021-08-04 16:27:58
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-05 16:28:54
 * @Description: file content
 * @FilePath: \sql\login_register\login.h
 */
#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "mainwindow.h"
#include "ui_login.h"
#include "ui_registerwin.h"
#include "registerwin.h"

#include "myqq.h"

#pragma once

namespace Ui
{
    class LOGIN;
}

class LOGIN : public QWidget
{
    Q_OBJECT

public:
    explicit LOGIN(QWidget *parent = nullptr);
    ~LOGIN();
    QString Username;
    QString Password;

signals:
    void toInputRoomNum(uint32_t roomId);

private slots:
    void on_ButtonLogin_clicked();

    void on_ButtonRegister_clicked();

    void on_InputRoomNum();

private:
    Ui::LOGIN *ui;
    MainWindow *Win;
    registerWin *register_win;
    MyQQ *myqq;
};

#endif // LOGIN_H
