#include "ServerConfig.h"

namespace hestia {
void WebAppConfig::load(const Metadata& items)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "identifier") {
            m_identifier = value;
        }
        else if (key == "interface") {
            if (value == "http") {
                m_interface = Interface::HTTP;
            }
            else if (value == "s3") {
                m_interface = Interface::S3;
            }
        }
    };
    items.for_each_item(on_item);
}

void ServerConfig::load(const Dictionary& config)
{
    auto server_conf = config.get_map_item("server");
    if (server_conf == nullptr) {
        return;
    }

    Metadata sub_config;
    server_conf->get_map_items(sub_config);

    std::string web_app_identifier;
    auto each_item = [this, &web_app_identifier](
                         const std::string& key, const std::string& value) {
        if (key == "host_address") {
            m_host = value;
        }
        else if (key == "host_port") {
            m_port = value;
        }
        else if (key == "controller_address") {
            m_controller_address = value;
        }
        else if (key == "web_app") {
            web_app_identifier = value;
        }
        else if (key == "backend") {
            m_backend = value;
        }
        else if (key == "static_resource_path") {
            m_static_resource_path = value;
        }
        else if (key == "cache_static_resources") {
            m_cache_static_resources = (value != "false");
        }
        else if (key == "tag") {
            m_tag = value;
        }
        else if (key == "controller") {
            m_controller = (value != "false");
        }
    };
    sub_config.for_each_item(each_item);

    if (!web_app_identifier.empty()) {
        if (auto web_app_conf = config.get_map_item("web_apps");
            web_app_conf != nullptr) {
            for (const auto& config : web_app_conf->get_sequence()) {
                std::string identifier;
                if (const auto identifier_dict =
                        config->get_map_item("identifier");
                    identifier_dict != nullptr) {
                    identifier = identifier_dict->get_scalar();
                }

                if (identifier == web_app_identifier) {
                    Metadata client_config;
                    config->get_map_items(client_config);
                    m_web_app_config.load(client_config);
                    break;
                }
            }
        }
    }
}
}  // namespace hestia