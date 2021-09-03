#pragma once

#include <QDebug>
#include <QWidget>

class AudioLevel : public QWidget {
    Q_OBJECT

public:
    explicit AudioLevel(QWidget *parent = nullptr);
    ~AudioLevel() override;
    void SetLevel(qreal level /*[0, 1.0]*/);
    void SetIsMuted(bool isMuted);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal level_ = 0.0;
    bool isMuted_ = false;
};
