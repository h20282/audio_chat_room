#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "audio/AudioChat.h"
#include "log/log.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    auto p = new AudioChat();
    for (auto device_name : p->GetInputDevices()) {
        LOG_ERROR("{}", device_name);
    }
    // p->JoinRoom("gzh", 122);
}

MainWindow::~MainWindow() {
    delete ui;
}
