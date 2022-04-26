/*
 * @Author: FengYanBin
 * @Date: 2021-08-06 21:15:48
 * @LastEditors: FengYanBin
 * @LastEditTime: 2021-08-14 11:51:05
 * @Description: file content
 * @FilePath: \sql\meet_room\roomdialog.h
 */
#pragma once

#include <QDialog>
#include <QPoint>
#include <QMenu>
#include "../common/customwidget.h"
#include "../audio/audiolevel.h"
#include "userwidget.h"
#include "userlist.h"

namespace Ui
{
    class RoomDialog;
}

class RoomDialog : public CustomMoveDialog
{
    Q_OBJECT

public:
    explicit RoomDialog(QWidget *parent = nullptr);
    ~RoomDialog();
    void setRoomid(const int &roomid);
    void setUserid(const QString &username);
    void setRoomOwner(const QString &owner);
    void setInfo(const int &roomid, const QString &username, const QString &owner);
    void addInputWidget(AudioLevel& level);
    void addUserWidget(UserWidget* user_widget);
    void removeUserWidget(UserWidget* user_widget);
    void setPb_openAudioText();
    QVBoxLayout* getuserListLayout();
    Ui::RoomDialog *getUi() const;
    void reCheckInputDevice(); // 重新检测输入设备

signals:
    void SIG_quitRoomSubmit();
    void SIG_unMute();
    void SIG_openAudio();
    void SIG_closeAudio();
    void SIG_refreshUserList();
    void SIG_setInputDevice(QAudioDeviceInfo deviceInfo);

private slots:
    void on_pb_min_clicked();
    void on_pb_quitRoom_clicked();
    void on_pb_close_clicked();
    void clear();
    void on_pb_openAudio_clicked();
    void on_pb_max_clicked();
    void on_pb_unmute_clicked();
    void on_pb_userlist_clicked();
    void on_reCheckBtn_clicked();
    void on_audioInputComboBox_activated(const QString &arg1);

private:
    Ui::RoomDialog *ui;
    UserListWidget *listwidget;

    int m_roomid;
    QString m_user_name;
};
