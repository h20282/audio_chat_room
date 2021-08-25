#include "registerwin.h"
#include "ui_registerwin.h"
#include"mainwindow.h"
#include"common.h"
#include"login.h"

#include<QMessageBox>
#include<QDebug>
#include<QDateTime>
#include <QCryptographicHash>

registerWin::registerWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerWin)
{
    ui->setupUi(this);
    this->setWindowTitle("注册");
    ui->lineEditEnsure->setEchoMode(QLineEdit::Password);
    ui->lineEditNew->setEchoMode(QLineEdit::Password);
}

registerWin::~registerWin()
{
    delete ui;
}

void registerWin::on_ButtonRegister_clicked()
{
    QString Username_register=ui->lineEditName->text();
    QString Password_register=ui->lineEditNew->text();
    QString Password_ensure=ui->lineEditEnsure->text();

    if(Username_register=="") {
        QMessageBox::information(nullptr,"提示","用户名为空！",QMessageBox::Yes);
        return;
    }

    if(Password_register=="") {
        QMessageBox::information(nullptr,"提示","输入密码为空！",QMessageBox::Yes);
        return;
    }

    if(Password_ensure!=Password_register) {
        QMessageBox::information(nullptr,"提示","两次输入密码不一致！",QMessageBox::Yes);
        return;
    }

    emit SIG_registerSubmit(Username_register, Password_register);
}

void registerWin::on_ButtonQuit_clicked()
{
    emit SIG_backToLoginInterface();
}
