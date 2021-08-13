#ifndef USERLIST_H
#define USERLIST_H

#include <QListWidget>
#include <QMouseEvent>
#include <QTextCodec>
#include <string>
#include <algorithm>
#include <iostream>

#include "useritem.h"


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

private:
    QPoint startPos;
    UserListWidgetItem *theHighlightItem = nullptr;
    UserListWidgetItem *oldHighlightItem = nullptr;
    UserListWidgetItem *theSelectedItem = nullptr;
    UserListWidgetItem *oldSelectedItem = nullptr;

private slots:
    void updateSelectedIcon();
};



#endif // USERLIST_H
