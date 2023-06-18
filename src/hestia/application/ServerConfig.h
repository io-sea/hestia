#pragma once

#include "Dictionary.h"
#include "Metadata.h"

namespace hestia {

struct WebAppConfig {
    enum class Interface { S3, HTTP };

    void load(const Metadata& items);

    static std::string to_string(Interface interface)
    {
        switch (interface) {
            case Interface::S3:
                return "s3";
            case Interface::HTTP:
                return "http";
        }
        return {};
    }

    std::string m_identifier{"hestia::HsmService"};
    Interface m_interface{Interface::HTTP};
};

struct ServerConfig {

    void load(const Dictionary& config);

    std::string m_host{"127.0.0.1"};
    std::string m_port{"8080"};
    std::string m_backend{"hestia::Basic"};
    std::string m_tag;
    std::string m_controller_address;
    std::string m_static_resource_path;
    bool m_cache_static_resources{true};
    bool m_controller{true};
    WebAppConfig m_web_app_config;
};
}  // namespace hestia