#include <QApplication>

#include "Config.h"
#include "audio/AudioChat.h"
#include "audio_server.h"
#include "log/log.h"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {
    {
        spdlog::set_pattern(kPattern);
        spdlog::set_level(spdlog::level::level_enum::debug);
    }
    QApplication a(argc, argv);

    AudioServer server;
    server.start();
    LOG_INFO("AudioServer started");
    /*
    for (int i = 0; i < 2; ++i) {
        auto p = new AudioChat();
        for (auto device_name : p->GetInputDevices()) {
            LOG_ERROR("{}", device_name);
        }
        p->JoinRoom("gzh", 122123);
    }
    */
    return a.exec();
}
