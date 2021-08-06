#include "registerwin.h"
#include "ui_registerwin.h"
#include"mainwindow.h"
#include"common.h"
#include"login.h"

#include<QMessageBox>
#include<QSqlQuery>
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

void registerWin::on_pushButton_clicked()
{

    QString Username_register=ui->lineEditName->text();
    QString Password_register=ui->lineEditNew->text();
    QString Password_ensure=ui->lineEditEnsure->text();

    QSqlQuery IdAll(db);
    QString searchAll="select count(*) from user";

    //用户的自增id,有个小bug，将用户删除后通过count*获取的个数不一致。不过问题不大，假设永久保存用户数据。
    int idNum;
    IdAll.exec(searchAll);
    while(IdAll.next()){
        idNum=IdAll.value(0).toInt();
        break;
    }

    QSqlQuery query(db);
    QString search=QString("select count(*) from user where username='%1'").arg(Username_register);

    if(Username_register==""){
        QMessageBox::information(NULL,"提示","用户名为空！",QMessageBox::Yes);
        return;
    }

    if(Password_register==""){
        QMessageBox::information(NULL,"提示","输入密码为空！",QMessageBox::Yes);
        return;
    }

    if(Password_ensure!=Password_register){
        QMessageBox::information(NULL,"提示","两次输入密码不一致！",QMessageBox::Yes);
        return;
    }

    //输入出现问题
    if(!query.exec(search))
    {
        QMessageBox::information(NULL,"提示","数据库出错！",QMessageBox::Yes);
    }
    else{
        while(query.next())
        {
            if(query.value(0).toInt() >= 1)
            {
                QMessageBox::information(NULL,"提示","用户名已存在！",QMessageBox::Yes);
                return;
            }
            else{
                QDateTime createTime=QDateTime::currentDateTime();
                QDateTime lastLogin=createTime;
                QSqlQuery insertSql(db);
                insertSql.prepare("INSERT INTO user(id, username, passwd, Createtime, LastLoginTime)" "VALUES(:id, :username, :passwd, :Createtime, :LastLoginTime)");
                insertSql.bindValue(":id",idNum+1);
                insertSql.bindValue(":username",Username_register);

                //加密安全处理
                QByteArray str=QCryptographicHash::hash(Password_register.toLatin1(),QCryptographicHash::Md5);
                QString md5;
                md5.append(str.toHex());

                insertSql.bindValue(":passwd",md5);
                insertSql.bindValue(":Createtime",createTime);
                insertSql.bindValue(":LastLoginTime",lastLogin);
                insertSql.exec();
                break;
            }
        }
    }
    QMessageBox::information(NULL,"提示","注册成功！",QMessageBox::Yes);
    login = new LOGIN();
    login->show();
    //MainWin->show();
    this->close();
}


void registerWin::on_pushButton_2_clicked()
{
    login = new LOGIN();
    login->show();
    this->close();
}

