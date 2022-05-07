#include "mainwindow.h"

#include "spdlog/spdlog.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    {
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%l][%s,%!]:%v");
        spdlog::set_level(spdlog::level::level_enum::debug);
    }
    QApplication a(argc, argv);
    MainWindow w;
    // w.show();
    return a.exec();
}
