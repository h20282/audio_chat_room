#pragma once

#include <memory>
#include <vector>

#include <QObject>
#include <QThread>

#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "controller.h"

class AudioServer : public QThread {
    Q_OBJECT

public:
    using WsServer = websocketpp::server<websocketpp::config::asio>;

public:
    AudioServer() = default;
    ~AudioServer() override;

public:
    void run() override;

private:
    WsServer server;
    std::vector<std::shared_ptr<Controller>> controllers_;
};
