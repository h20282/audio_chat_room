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
    UserWidget(QString userName, AudioChat *auidioChat, QWidget* parent = nullptr)
        : QWidget(parent)
        , m_userName(userName)
        , m_auidioChat(auidioChat)
    {
        m_volume = 100;
        m_isMuted = false;

        m_layout = new QVBoxLayout(this);

        this->setMaximumHeight(80);

        //setStyleSheet(QString::fromUtf8("color:white"));

        //setStyleSheet(QString::fromUtf8("border:1px solid black"));
        //setStyleSheet(QString::fromUtf8("background-color:gray"));




        m_label = new QLabel(this);
        m_label->setText(QString("%1").arg(userName.toLatin1().data()));
        m_label->setMaximumHeight(80);
        m_layout->addWidget(m_label);


        m_slider = new QSlider(Qt::Orientation::Horizontal, this);
        m_slider->setRange(0, 200);
        m_slider->setSingleStep(1);
        m_slider->setValue(100);

        m_level = new AudioLevel(this);




        m_layout->addWidget(m_slider);
        m_layout->addWidget(m_level);
        m_layout->setStretch(2,1);
        m_layout->setMargin(0);
        setLayout(m_layout);

        connect(this->m_slider, &QSlider::sliderMoved, [this](int val){
            emit sig_sliderMoved(val);
        });
    }
    ~UserWidget(){
        delete m_slider;
        delete m_label;
        delete m_layout;
        delete m_level;
    }

    void setVol(double vol){
        m_level->setLevel(vol);
    }



signals:
    void sig_sliderMoved(int val);

private:
    QString m_userName;
    double m_volume;
    bool m_isMuted;

    QLabel *m_label;
    QSlider *m_slider;
    AudioLevel *m_level;

    QVBoxLayout *m_layout;

    AudioChat *m_auidioChat;

};

#endif // USERWIDGET_H
