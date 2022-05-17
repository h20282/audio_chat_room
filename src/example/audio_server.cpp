#include "audio_server.h"

#include <QObject>

AudioServer::~AudioServer() {}

void AudioServer::run() {
    server.set_open_handler([&](websocketpp::connection_hdl hd) {
        auto con = server.get_con_from_hdl(hd);
        QMetaObject::invokeMethod(this, [&, con] {
            controllers_.push_back(std::make_shared<Controller>(con));
        });
    });
    server.set_access_channels(websocketpp::log::alevel::all);
    server.set_error_channels(websocketpp::log::elevel::all);

    server.clear_access_channels(websocketpp::log::alevel::all);

    server.init_asio();
    server.listen(9007);
    server.start_accept();

    server.run();
}
