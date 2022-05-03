/*
 * @Author: FengYanBin
 * @Date: 2021-08-04 16:27:58
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-09 16:15:57
 * @Description: file content
 * @FilePath: \sql\login_register\login.h
 */
#pragma once

#include <QWidget>
#include "mainwindow.h"
#include "ui_login.h"
#include "ui_registerwin.h"
#include "registerwin.h"

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
    void SIG_loginSubmit(QString name, QString password);

    void SIG_JumpToRegisterInterface();

    void SIG_SkipLogin();

private slots:
    void on_ButtonLogin_clicked();

    void on_ButtonRegister_clicked();

    void on_ButtonPass_clicked();

private:
    Ui::LOGIN *ui;
    MainWindow *Win;
    registerWin *register_win;
};