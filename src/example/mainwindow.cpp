#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "audio/AudioChat.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    auto p = new AudioChat();
    p->JoinRoom("gzh", 122);
}

MainWindow::~MainWindow() {
    delete ui;
}
