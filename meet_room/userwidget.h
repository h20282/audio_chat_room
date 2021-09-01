#ifndef USERWIDGET_H
#define USERWIDGET_H

#include "./audio/audiolevel.h"
#include "./audio/AudioChat.h"

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QSlider>

class UserWidget : public QWidget
{
    Q_OBJECT
public:
    UserWidget(QString userName, QWidget* parent = nullptr);
    ~UserWidget();
    void setVol(double vol);
    void setIsMuted(bool isMuted);

signals:
    void sig_sliderMoved(int val);

private:
    QLabel *m_label;
    QSlider *m_slider;
    AudioLevel *m_level;
    QVBoxLayout *m_layout;
};

#endif // USERWIDGET_H
