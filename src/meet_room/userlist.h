#pragma once

#include <QListWidget>
#include <QMouseEvent>
#include <QTextCodec>
#include <QMessageBox>
#include <QDebug>
#include <string>
#include <algorithm>
#include <iostream>

#include "useritem.h"
#include "algorithm"
#include <string>



class UserListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit UserListWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void contextMenuEvent (QContextMenuEvent * event );

private:
    QPoint startPos;
    int GetNum(QString text);
    QString GetName(QString text);
    UserListWidgetItem *theHighlightItem = nullptr;
    UserListWidgetItem *oldHighlightItem = nullptr;
    UserListWidgetItem *theSelectedItem = nullptr;
    UserListWidgetItem *oldSelectedItem = nullptr;

    int user_id;
    QString user_name;

signals:
    void SIG_muteUser(int);

    void SIG_transferUser(int);

    void SIG_kick_out_ofUser(int);

private slots:
    void updateSelectedIcon();

    void muteItem();

    void transferItem();

    void kick_out_of_roomItem();
};

class RoomListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit RoomListWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void contextMenuEvent (QContextMenuEvent * event );

signals:
    void SIG_joinRoom(int);

private:
    QPoint startPos;
    int GetNum(QString text);
    RoomListWidgetItem *theHighlightItem = nullptr;
    RoomListWidgetItem *oldHighlightItem = nullptr;
    RoomListWidgetItem *theSelectedItem = nullptr;
    RoomListWidgetItem *oldSelectedItem = nullptr;

    int room_num;

private slots:
    void updateSelectedIcon();

    void joinItem();
};
