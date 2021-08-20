#include "userlist.h"

UserListWidget::UserListWidget(QWidget *parent) :
    QListWidget(parent)
{
    //mouseMoveEvent(QMouseEvent *event);//需要该属性
    setMouseTracking(true);
   // 当前被选中项发生变化时，触发item图标的更新
    connect(this, &UserListWidget::itemSelectionChanged, this, &UserListWidget::updateSelectedIcon);
}

//处理鼠标hover时，item的图标需要变成hover状态
void UserListWidget::mouseMoveEvent(QMouseEvent *event)
{
}

//鼠标按下时，startPos记录单击位置
void UserListWidget::mousePressEvent(QMouseEvent *event)
{
    startPos = event->pos();
}

//释放鼠标时，item才会被选中
void UserListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //如果鼠标释放位置和单击位置相距超过5像素，则不会触发item选中
    if((event->pos() - startPos).manhattanLength() > 5) return;
}

//处理鼠标离开后，hover图标回复正常状态
void UserListWidget::leaveEvent(QEvent *event)
{

}

void UserListWidget::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu* popMenu = new QMenu(this);

        if(this->itemAt(mapFromGlobal(QCursor::pos())) != NULL) //如果有item则添加"修改"菜单 [1]*
        {
            QString str = this->itemAt(mapFromGlobal(QCursor::pos()))->text();
            this->user_id = GetNum(str);
            this->user_name = GetName(str);
            popMenu->addAction("静音用户", this, SLOT(muteItem()));
            popMenu->addAction("转让房主", this, SLOT(transferItem()));
            popMenu->addAction("踢出房间", this, SLOT(kick_out_of_roomItem()));
            popMenu->addAction("调节音量", this, SLOT(adjustVolumnItem()));
            popMenu->addAction("屏蔽用户", this, SLOT(closeVolumn()));
            popMenu->addAction("解除屏蔽", this, SLOT(unBlock()));
        }

        popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}

int UserListWidget::GetNum(QString text)
{
    std::string str = text.toStdString();
    int index1 = str.find("'");
    str = str.substr(index1 + 1);
    int index2 = str.find("'");
    str = str.substr(0, index2);
    return stoi(str);
}

QString UserListWidget::GetName(QString text)
{
    std::string str = text.toStdString();
    int index1 = str.rfind("'");
    str = str.substr(0, index1);
    int index2 = str.rfind("'");
    str = str.substr(index2 + 1);
    return QString::fromStdString(str);
}

//响应itemSelectionChanged()信号，处理item被选中后，图标变成并保持hover状态
void UserListWidget::updateSelectedIcon()
{
    oldSelectedItem = theSelectedItem;
    theSelectedItem = static_cast<UserListWidgetItem *>(currentItem());

    //之前被选中的item图标回复原样
    //新被选中的item图标变成hover状态
    if(oldSelectedItem != theSelectedItem){
        if(oldSelectedItem) oldSelectedItem->setIcon(oldSelectedItem->Img);
        if(theSelectedItem) theSelectedItem->setIcon(theSelectedItem->Img_hover);
    }
}

void UserListWidget::muteItem()
{

    emit SIG_muteUser(this->user_id);
}

void UserListWidget::transferItem()
{
    if( QMessageBox::question( this , "提示","是否转让房主给此人?") == QMessageBox::Yes )
    {
        qDebug() << "转让房主！" << endl;
        qDebug() << "transfer_num=" << this->user_id << endl;
        emit SIG_transferUser(this->user_id);
        //this->close();
    }
}

void UserListWidget::kick_out_of_roomItem()
{
    if( QMessageBox::question( this , "提示","是否将此人踢出房间?") == QMessageBox::Yes )
    {
        emit SIG_kick_out_ofUser(this->user_id);
    }
}

void UserListWidget::adjustVolumnItem()
{
    emit SIG_adjustVolumnUser(this->user_id);
}

void UserListWidget::closeVolumn()
{
    emit SIG_closeVolumn(this->user_name);
}

void UserListWidget::unBlock()
{
    emit SIG_unBlock(this->user_name);
}


RoomListWidget::RoomListWidget(QWidget *parent) :
    QListWidget(parent)
{
    //mouseMoveEvent(QMouseEvent *event);//需要该属性
    setMouseTracking(true);
   // 当前被选中项发生变化时，触发item图标的更新
    connect(this, &RoomListWidget::itemSelectionChanged, this, &RoomListWidget::updateSelectedIcon);
}

//处理鼠标hover时，item的图标需要变成hover状态
void RoomListWidget::mouseMoveEvent(QMouseEvent *event)
{
//    oldHighlightItem = theHighlightItem;
//    theHighlightItem = static_cast<UserListWidgetItem *>(itemAt(event->pos()));

//    //旧的hover的item图标回复原状(条件是该item没有被选中)
//    //新的hover的iten图标变成hover状态(条件是该item没有被选中)
//    if(oldHighlightItem != theHighlightItem){
//        if(oldHighlightItem && !oldHighlightItem->isSelected()) oldHighlightItem->setIcon(oldHighlightItem->Img);
//        if(theHighlightItem && !theHighlightItem->isSelected()) theHighlightItem->setIcon(theHighlightItem->Img_hover);
//    }
}

//鼠标按下时，startPos记录单击位置
void RoomListWidget::mousePressEvent(QMouseEvent *event)
{
    startPos = event->pos();
}

//释放鼠标时，item才会被选中
void RoomListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //如果鼠标释放位置和单击位置相距超过5像素，则不会触发item选中
    if((event->pos() - startPos).manhattanLength() > 5) return;
}

//处理鼠标离开后，hover图标回复正常状态
void RoomListWidget::leaveEvent(QEvent *event)
{
//    Q_UNUSED(event);
//    oldHighlightItem = theHighlightItem;
//    if(oldHighlightItem && !oldHighlightItem->isSelected()) oldHighlightItem->setIcon(oldHighlightItem->Img);
    //    oldHighlightItem = theHighlightItem = nullptr;
}

void RoomListWidget::contextMenuEvent(QContextMenuEvent *event)
{
        QMenu* popMenu = new QMenu(this);

        if(this->itemAt(mapFromGlobal(QCursor::pos())) != NULL) //如果有item则添加"修改"菜单 [1]*
        {
            QString str = this->itemAt(mapFromGlobal(QCursor::pos()))->text();
            this->room_num = GetNum(str);
            popMenu->addAction("加入房间", this, SLOT(joinItem()));

        }

        popMenu->exec(QCursor::pos()); // 菜单出现的位置为当前鼠标的位置
}

int RoomListWidget::GetNum(QString text)
{
    std::string str = text.toStdString();
    int index1 = str.find("'");
    str = str.substr(index1 + 1);
    int index2 = str.find("'");
    str = str.substr(0, index2);
    return stoi(str);
}

//响应itemSelectionChanged()信号，处理item被选中后，图标变成并保持hover状态
void RoomListWidget::updateSelectedIcon()
{
    oldSelectedItem = theSelectedItem;
    theSelectedItem = static_cast<RoomListWidgetItem *>(currentItem());

    //之前被选中的item图标回复原样
    //新被选中的item图标变成hover状态
    if(oldSelectedItem != theSelectedItem){
        if(oldSelectedItem) oldSelectedItem->setIcon(oldSelectedItem->Img);
        if(theSelectedItem) theSelectedItem->setIcon(theSelectedItem->Img_hover);
    }
}

void RoomListWidget::joinItem()
{
    if( QMessageBox::question( this , "提示","是否加入房间?") == QMessageBox::Yes )
    {
//        QString str = this->itemAt(mapFromGlobal(QCursor::pos()))->text();
//        int num = GetNum(str);
        qDebug() << "num=" << this->room_num << endl;
        emit SIG_joinRoom(this->room_num);
    }
}



