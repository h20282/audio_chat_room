/*
 * @Author: FengYanBin
 * @Date: 2021-08-05 15:00:11
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-05 16:31:23
 * @Description: file content
 * @FilePath: \sql\myqq.h
 */
#pragma once

#include "./meet_room/roomdialog.h"

#include <QWidget>
#include <QDebug>
#include <QMessageBox>

namespace Ui
{
    class MyQQ;
}

class MyQQ : public QWidget
{
    Q_OBJECT

public:
    explicit MyQQ(QWidget *parent = nullptr);
    ~MyQQ();

signals:
    void toEnterRoom(uint32_t roomId);

private slots:
    void OnToEnterRoom();

    void on_pb_search_clicked();

    void on_le_search_returnPressed();

private:
    Ui::MyQQ *ui;
    RoomDialog *roomdialog;
};
