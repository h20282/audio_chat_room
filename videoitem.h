#pragma once

#include <QWidget>

namespace Ui {
class VideoItem;
}

class VideoItem : public QWidget
{
    Q_OBJECT

public:
    explicit VideoItem(QWidget *parent = nullptr);
    ~VideoItem();

private:
    Ui::VideoItem *ui;
};
