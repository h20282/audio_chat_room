#pragma once

#include <QListWidgetItem>
#include <QListWidget>
#include <QIcon>
#include <QMenu>

class UserListWidgetItem : public QListWidgetItem
{
    //Q_OBJECT  //由于QListWidgetItem没有QObject属性，所以Q_OBJECT需要注释掉
public:
    explicit UserListWidgetItem(QListWidget *view = nullptr);
    void setUpIcon(const QIcon &icon, const QIcon &icon_hover);


    QIcon Img;
    QIcon Img_hover;
};

class RoomListWidgetItem : public QListWidgetItem
{
    //Q_OBJECT  //由于QListWidgetItem没有QObject属性，所以Q_OBJECT需要注释掉
public:
    explicit RoomListWidgetItem(QListWidget *view = nullptr);
    void setUpIcon(const QIcon &icon, const QIcon &icon_hover);


    QIcon Img;
    QIcon Img_hover;
};

