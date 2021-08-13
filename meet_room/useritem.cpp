#include "useritem.h"

UserListWidgetItem::UserListWidgetItem(QListWidget *view) :
    QListWidgetItem(view)
{
}

void UserListWidgetItem::setUpIcon(const QIcon &icon, const QIcon &icon_hover)
{
    Img = icon;
    Img_hover = icon_hover;
    setIcon(Img);
}


