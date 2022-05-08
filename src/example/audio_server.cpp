#include "audio_server.h"

AudioServer::~AudioServer() {}

void AudioServer::run() {
    server.set_open_handler([&](websocketpp::connection_hdl hd) {
        auto con = server.get_con_from_hdl(hd);
        pairs_.push_back(std::make_shared<Pair>(con));
    });
    server.set_access_channels(websocketpp::log::alevel::all);
    server.set_error_channels(websocketpp::log::elevel::all);

    server.init_asio();
    server.listen(9007);
    server.start_accept();

    server.run();
}
