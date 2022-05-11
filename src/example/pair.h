#pragma once

#include <QObject>
#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "audio/AudioChat.h"

class Pair : public QObject {
    Q_OBJECT

public:
    using Connection =
            websocketpp::server<websocketpp::config::asio>::connection_ptr;

public:
    Pair() = delete;
    ~Pair() = default;

    Pair(Connection con);

public:
    /*QString user_name, int room_id) */
    void JoinRoom(const nlohmann::json &);
    void LeaveRoom(const nlohmann::json &);
    /*QString device_name*/
    void SetInputDevice(const nlohmann::json &);
    // ->/*std::set<std::string>*/
    void GetInputDevices(const nlohmann::json &);
    /*QString name, int volume [0,200]*/
    void SetUserVolume(const nlohmann::json &);
    /*bool is_muted*/
    void SetMuted(const nlohmann::json &);

private:
    void Init();

private:
    Connection con_;
    std::shared_ptr<AudioChat> audio_chat_;
};
