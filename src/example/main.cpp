#include "mainwindow.h"

#include "Config.h"
#include "audio/AudioChat.h"
#include "log/log.h"
#include "spdlog/spdlog.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    {
        spdlog::set_pattern(kPattern);
        spdlog::set_level(spdlog::level::level_enum::debug);
    }
    QApplication a(argc, argv);
    MainWindow w;
    // w.show();
    return a.exec();
}
