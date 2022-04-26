/*
 * @Author: FengYanBin
 * @Date: 2021-08-04 16:28:12
 * @LastEditTime: 2021-08-05 16:29:07
 * @LastEditors: FengYanBin
 * @Description: In User Settings Edit
 * @FilePath: \sql\login_register\login.cpp
 */
#include "login.h"
#include "ui_login.h"
#include "mainwindow.h"
#include "registerwin.h"
#include "common.h"
#include <QDebug>
#include <QMessageBox>
#include <QCryptographicHash>


LOGIN::LOGIN(QWidget *parent) : QWidget(parent),
                                ui(new Ui::LOGIN)
{
    ui->setupUi(this);
    this->setWindowTitle("登录");
    ui->lineEditPassword->setEchoMode(QLineEdit::Password); //密码输入模式，隐藏
}

LOGIN::~LOGIN()
{
    delete ui;
}

void LOGIN::on_ButtonLogin_clicked()
{
    QString Username_tmp = ui->lineEditName->text();
    QString Password_tmp = ui->lineEditPassword->text();

    emit SIG_loginSubmit(Username_tmp, Password_tmp);
}

void LOGIN::on_ButtonRegister_clicked()
{
    emit SIG_JumpToRegisterInterface();
}


void LOGIN::on_ButtonPass_clicked()
{
    emit SIG_SkipLogin();
}
