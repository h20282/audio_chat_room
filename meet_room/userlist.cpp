#include "userlist.h"

UserListWidget::UserListWidget(QWidget *parent) :
    QListWidget(parent)
{
    //mouseMoveEvent(QMouseEvent *event)需要该属性
    setMouseTracking(true);
    //当前被选中项发生变化时，触发item图标的更新
    connect(this, &UserListWidget::itemSelectionChanged, this, &UserListWidget::updateSelectedIcon);
}

//处理鼠标hover时，item的图标需要变成hover状态
void UserListWidget::mouseMoveEvent(QMouseEvent *event)
{
    oldHighlightItem = theHighlightItem;
    theHighlightItem = static_cast<UserListWidgetItem *>(itemAt(event->pos()));

    //旧的hover的item图标回复原状(条件是该item没有被选中)
    //新的hover的iten图标变成hover状态(条件是该item没有被选中)
    if(oldHighlightItem != theHighlightItem){
        if(oldHighlightItem && !oldHighlightItem->isSelected()) oldHighlightItem->setIcon(oldHighlightItem->Img);
        if(theHighlightItem && !theHighlightItem->isSelected()) theHighlightItem->setIcon(theHighlightItem->Img_hover);
    }
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

    UserListWidgetItem *item = static_cast<UserListWidgetItem *>(itemAt(event->pos()));
    setCurrentItem(item);

//    QString text = item->text();
//    std::string str = text.toStdString();
//    QTextCodec* pCode = QTextCodec::codecForName("gb18030");
//    if (nullptr != pCode)
//    {
//       str = pCode->fromUnicode(text).data();
//    }

//    int index1 = str.rfind("'");
//    str = str.substr(0, index1);
//    int index2 = str.rfind("'");
//    str = str.substr(index2 + 1);


//    //TODO:发出信号，提示可以踢人或者静音
//    if (event->button() & Qt::RightButton) {

//    } else {

//    }
}

//处理鼠标离开后，hover图标回复正常状态
void UserListWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    oldHighlightItem = theHighlightItem;
    if(oldHighlightItem && !oldHighlightItem->isSelected()) oldHighlightItem->setIcon(oldHighlightItem->Img);
    oldHighlightItem = theHighlightItem = nullptr;
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
