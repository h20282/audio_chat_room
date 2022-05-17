#include "pair.h"

#include <QMetaObject>
#include <vector>

#include "log/log.h"

Controller::Controller(Connection con) : con_(con) {

    con_->set_message_handler(
            [this](websocketpp::connection_hdl hd,
                   websocketpp::server<websocketpp::config::asio>::message_ptr
                           message) {
                auto msg = message->get_payload();
                try {

                    auto json = nlohmann::json::parse(msg);

                    auto type = json.at("type").get<std::string>();
                    const static std::map<
                            std::string, void (Controller::*)(const nlohmann::json &)>
                            handlers = {
                                    {"JoinRoom", &Controller::JoinRoom},
                                    {"LeaveRoom", &Controller::LeaveRoom},
                                    {"SetInputDevice", &Controller::SetInputDevice},
                                    {"GetInputDevices", &Controller::GetInputDevices},
                                    {"SetUserVolume", &Controller::SetUserVolume},
                                    {"SetMuted", &Controller::SetMuted},
                            };
                    auto handler = handlers.at(type);
                    (this->*handler)(json);
                } catch (nlohmann::json::exception e) {
                    LOG_ERROR("{} when hand msg: `{}`", e.what(), msg);
                } catch (std::exception e) {
                    LOG_ERROR("{} when hand msg: `{}`", e.what(), msg);
                }
            });
    con_->set_close_handler(
            [this](websocketpp::connection_hdl) { this->LeaveRoom({}); });
}

///*QString user_name, int room_id) */
void Controller::JoinRoom(const nlohmann::json &msg) {
    QMetaObject::invokeMethod(this, [&]() { Init(); });
    auto user_name = msg.at("user_name").get<std::string>();
    auto room_id = msg.at("room_id").get<uint32_t>();
    LOG_INFO("user_name: {}, room_id: ", user_name, room_id);
    QMetaObject::invokeMethod(this, [=]() {
        LOG_INFO("user_name: {}, room_id: ", user_name, room_id);
        audio_chat_->JoinRoom(QString(user_name.c_str()), room_id);
    });
}
void Controller::LeaveRoom(const nlohmann::json &msg) {
    QMetaObject::invokeMethod(this, [&] { audio_chat_.reset(); });
}
///*QString device_name*/
void Controller::SetInputDevice(const nlohmann::json &msg) {
    auto device_name = msg.at("device_name").get<std::string>();
    QMetaObject::invokeMethod(this, [=]() {
        LOG_INFO("device_name: {}", device_name);
        audio_chat_->SetInputDevice(QString(device_name.c_str()));
    });
}
//// ->/*std::set<std::string>*/
void Controller::GetInputDevices(const nlohmann::json &msg) {
    nlohmann::json ret;

    for (auto device_name : audio_chat_->GetInputDevices()) {
        ret.push_back(device_name);
    }
    nlohmann::json reply{{"type", "ret.GetInputDevices"}, {"ret", ret}};
    con_->send(reply.dump());
}
///*QString name, int volume [0,200]*/
void Controller::SetUserVolume(const nlohmann::json &msg) {
    auto name = msg.at("name").get<std::string>();
    auto volume = msg.at("volume").get<std::string>();
    LOG_INFO("name:{}, volume: {}", name, volume);
    audio_chat_->SetUserVolume(QString(name.c_str()), stoi(volume));
}
///*bool is_muted*/
void Controller::SetMuted(const nlohmann::json &msg) {
    auto is_muted = msg.at("is_muted").get<bool>();
    audio_chat_->SetMuted(is_muted);
}

void Controller::Init() {
    LOG_INFO("new AudioChat");
    audio_chat_.reset(new AudioChat());

    QObject::connect(audio_chat_.get(), &AudioChat::SigCollectorVolumeReady,
                     [this](double volume) {
                         nlohmann::json msg = {
                                 {"type", "SigCollectorVolumeReady"},
                                 {"volume", volume},
                         };
                         con_->send(msg.dump());
                         LOG_TRACE("volume: {}", volume);
                     });

    QObject::connect(audio_chat_.get(), &AudioChat::SigUserVolumeReady,
                     [this](QString name, double volume) {
                         nlohmann::json msg = {
                                 {"type", "SigUserVolumeReady"},
                                 {"name", name.toStdString()},
                                 {"volume", volume},
                         };
                         con_->send(msg.dump());
                         LOG_TRACE("name: {}, volume: {}", name.toStdString(),
                                   volume);
                     });

    QObject::connect(audio_chat_.get(), &AudioChat::SigUserListReady,
                     [this](QList<QString> list) {
                         for (auto v : list) { LOG_ERROR("v"); }
                     });

    QObject::connect(audio_chat_.get(), &AudioChat::SigUserIsMutedStatusReady,
                     [](QMap<QString, bool> user_status) {
                         LOG_TRACE("SigUserIsMutedStatusReady");
                     });
}
