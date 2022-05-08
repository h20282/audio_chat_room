#include "pair.h"

#include <QMetaObject>
#include <vector>

#include "log/log.h"

Pair::Pair(Connection con) : con_(con) {

    QMetaObject::invokeMethod(this, [&]() {
        LOG_INFO("new AudioChat");
        audio_chat_.reset(new AudioChat());
    });

    QObject::connect(audio_chat_.get(), &AudioChat::SigCollectorVolumeReady,
                     [](double volume) { LOG_INFO("volume: {}", volume); });

    QObject::connect(audio_chat_.get(), &AudioChat::SigUserVolumeReady,
                     [](QString name, double volume) {
                         LOG_INFO("name: {}, volume: {}", name.toStdString(),
                                  volume);
                     });

    QObject::connect(audio_chat_.get(), &AudioChat::SigUserListReady,
                     [](QList<QString> list) {
                         for (auto v : list) { LOG_ERROR("v"); }
                     });

    QObject::connect(audio_chat_.get(), &AudioChat::SigUserIsMutedStatusReady,
                     [](QMap<QString, bool> user_status) {
                         LOG_INFO("SigUserIsMutedStatusReady");
                     });

    con_->set_message_handler(
            [this](websocketpp::connection_hdl hd,
                   websocketpp::server<websocketpp::config::asio>::message_ptr
                           message) {
                auto msg = message->get_payload();
                try {

                    auto json = nlohmann::json::parse(msg);

                    auto type = json.at("type").get<std::string>();
                    const static std::map<
                            std::string, void (Pair::*)(const nlohmann::json &)>
                            handlers = {
                                    {"JoinRoom", &Pair::JoinRoom},
                                    {"SetInputDevice", &Pair::SetInputDevice},
                                    {"GetInputDevices", &Pair::GetInputDevices},
                                    {"SetUserVolume", &Pair::SetUserVolume},
                                    {"SetMuted", &Pair::SetMuted},
                            };
                    auto handler = handlers.at(type);
                    (this->*handler)(json);
                } catch (nlohmann::json::exception e) {
                    LOG_ERROR("{} when hand msg: `{}`", e.what(), msg);
                } catch (std::exception e) {
                    LOG_ERROR("{} when hand msg: `{}`", e.what(), msg);
                }
            });
    con_->set_close_handler([this](websocketpp::connection_hdl) {
        QMetaObject::invokeMethod(this, [&] { audio_chat_.reset(); });
    });
}

///*QString user_name, int room_id) */
void Pair::JoinRoom(const nlohmann::json &msg) {
    auto user_name = msg.at("user_name").get<std::string>();
    auto room_id = msg.at("room_id").get<uint32_t>();
    LOG_INFO("user_name: {}, room_id: ", user_name, room_id);
    QMetaObject::invokeMethod(this, [=]() {
        LOG_INFO("user_name: {}, room_id: ", user_name, room_id);
        audio_chat_->JoinRoom(QString(user_name.c_str()), room_id);
    });
}
///*QString device_name*/
void Pair::SetInputDevice(const nlohmann::json &msg) {
    auto device_name = msg.at("device_name").get<std::string>();
    audio_chat_->SetInputDevice(QString(device_name.c_str()));
}
//// ->/*std::set<std::string>*/
void Pair::GetInputDevices(const nlohmann::json &msg) {
    std::vector<std::string> device_names;
    for (auto device_name : audio_chat_->GetInputDevices()) {
        device_names.push_back(device_name);
    }
    nlohmann::json reply{
            // TODO fill the list
    };
    con_->send(reply.dump());
}
///*QString name, int volume [0,200]*/
void Pair::SetUserVolume(const nlohmann::json &msg) {
    auto name = msg.at("name").get<std::string>();
    auto volume = msg.at("volume").get<double>();
    audio_chat_->SetUserVolume(QString(name.c_str()), volume);
}
///*bool is_muted*/
void Pair::SetMuted(const nlohmann::json &msg) {
    auto is_muted = msg.at("is_muted").get<bool>();
    audio_chat_->SetMuted(is_muted);
}
