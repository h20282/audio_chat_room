#pragma once

#include <QLabel>
#include <QLayout>
#include <QSlider>
#include <QWidget>

#include "./audio/AudioChat.h"
#include "./audio/audiolevel.h"

class UserWidget : public QWidget {
    Q_OBJECT
public:
    UserWidget(QString userName, QWidget *parent = nullptr);
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
