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
    UserWidget(QString userName, QWidget* parent = nullptr)
        : QWidget(parent)
    {
        m_layout = new QVBoxLayout(this);

        this->setMaximumHeight(80);

        m_label = new QLabel(this);
        m_label->setText(QString("%1").arg(userName.toLatin1().data()));
        m_label->setMaximumHeight(80);

        m_slider = new QSlider(Qt::Orientation::Horizontal, this);
        m_slider->setRange(0, 200);
        m_slider->setSingleStep(1);
        m_slider->setValue(100);

        m_level = new AudioLevel(this);

        m_layout->addWidget(m_label);
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

    void setIsMuted(bool isMuted){
        this->m_level->setIsMuted(isMuted);
    }

signals:
    void sig_sliderMoved(int val);

private:
    QLabel *m_label;
    QSlider *m_slider;
    AudioLevel *m_level;

    QVBoxLayout *m_layout;
};

#endif // USERWIDGET_H
