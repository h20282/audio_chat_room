#ifndef REGISTERWIN_H
#define REGISTERWIN_H

#include <QWidget>
#include"mainwindow.h"
#include "ui_login.h"
#include "ui_registerwin.h"
//#include "login.h"
#pragma once
class LOGIN;

namespace Ui {
class registerWin;
}

class registerWin : public QWidget
{
    Q_OBJECT

public:
    explicit registerWin(QWidget *parent = nullptr);
    ~registerWin();

signals:
    void SIG_registerSubmit(QString name , QString passord);

    void SIG_backToLoginInterface();

private slots:
    void on_ButtonRegister_clicked();

    void on_ButtonQuit_clicked();

private:
    Ui::registerWin *ui;
    MainWindow *MainWin;
    LOGIN* login;
};

#endif // REGISTERWIN_H
