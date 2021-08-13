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
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QCryptographicHash>

QString GlobalUsername;
QString GlobalPassword;

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
    //qDebug() << "发出登录请求！" << endl;
    QString Username_tmp = ui->lineEditName->text();
    QString Password_tmp = ui->lineEditPassword->text();

    emit SIG_loginSubmit(Username_tmp, Password_tmp);

//    QString md5;
//    QByteArray str = QCryptographicHash::hash(Password_tmp.toLatin1(), QCryptographicHash::Md5);
//    md5.append(str.toHex());
//    //qDebug() << "md5:" << md5 << endl;

//    QSqlQuery query(db);
//    QString search = QString("select count(*) from user where username='%1' and passwd='%2'").arg(Username_tmp).arg(md5);
//    //qDebug() << "md5:" << search << endl;
//    if (!query.exec(search))
//    {
//        QMessageBox::information(NULL, "提示", "数据库出错！", QMessageBox::Yes);
//    }
//    else
//    {
//        while (query.next())
//        {
//            if (query.value(0).toInt() == 1)
//            {
//                GlobalUsername = Username_tmp;
//                GlobalPassword = md5;
//                qDebug() << "登陆成功！" << endl;
//                //TODO:登录后修改最后登录时间
//                emit toInputRoomNum(123456);
//                //                Win = new MainWindow();
//                //                Win->show();
//                this->close();
//                break;
//            }
//            else
//            {
//                QMessageBox::information(NULL, "提示", "用户名或密码错误！", QMessageBox::Yes);
//                break;
//            }
//        }
//    }
}

void LOGIN::on_ButtonRegister_clicked()
{

    emit SIG_JumpToRegisterInterface();
//    register_win = new registerWin();
//    register_win->show();
//    this->close();
}


void LOGIN::on_ButtonPass_clicked()
{
    qDebug() <<"skip" << endl;
    emit SIG_SkipLogin();
}
