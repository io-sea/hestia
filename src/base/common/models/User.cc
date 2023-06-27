#include "User.h"

namespace hestia {
User::User() : Model("user") {}

User::User(const Uuid& id) : Model(id) {}

User::User(const std::string& name) : Model(name, "user") {}

void User::deserialize(const Dictionary& dict, SerializeFormat format)
{
    Model::deserialize(dict, format);

    if (format == SerializeFormat::ID) {
        return;
    }

    Metadata scalar_data;
    dict.get_map_items(scalar_data);

    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "password" && !value.empty()) {
            m_password = value;
        }
        else if (key == "display_name" && !value.empty()) {
            m_display_name = value;
        }
        else if (key == "is_admin" && !value.empty()) {
            m_is_admin = value == "true";
        }
    };
    scalar_data.for_each_item(on_item);

    if (dict.has_map_item("token")) {
        Metadata token_data;
        dict.get_map_item("token")->get_map_items(token_data);
        auto on_token_item =
            [this](const std::string& key, const std::string& value) {
                if (key == "created" && !value.empty()) {
                    m_api_token.m_creation_time = std::stoull(value);
                }
                else if (key == "value" && !value.empty()) {
                    m_api_token.m_value = value;
                }
            };
        token_data.for_each_item(on_token_item);
    }
}

void User::serialize(
    Dictionary& dict, SerializeFormat format, const Uuid& id) const
{
    Model::serialize(dict, format, id);

    if (format == SerializeFormat::ID) {
        return;
    }

    std::unordered_map<std::string, std::string> data{
        {"password", m_password},
        {"display_name", m_display_name},
        {"is_admin", m_is_admin ? "true" : "false"},
    };

    dict.set_map(data);

    auto token_dict = std::make_unique<Dictionary>();
    std::unordered_map<std::string, std::string> token_data{
        {"value", m_api_token.m_value},
        {"created", std::to_string(m_api_token.m_creation_time)}};
    token_dict->set_map(token_data);
    dict.set_map_item("token", std::move(token_dict));
}
}  // namespace hestia