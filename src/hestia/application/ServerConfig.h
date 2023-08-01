#pragma once

#include "WebAppConfig.h"

namespace hestia {

class ServerConfig : public SerializeableWithFields {

  public:
    ServerConfig();

    ServerConfig(const ServerConfig& other);

    const std::string& get_static_resource_path() const;

    const std::string& get_host_address() const;

    unsigned get_port() const;

    const std::string& get_tag() const { return m_tag.get_value(); }

    const WebAppConfig& get_web_app_config() const;

    static std::string get_type() { return s_type; }

    const std::string& get_controller_address() const;

    const std::string& get_api_prefix() const;

    bool has_controller_address() const;

    void set_controller_address(const std::string& host, unsigned port)
    {
        m_controller_address.update_value(host + ":" + std::to_string(port));
    }

    bool is_controller() const;

    bool should_cache_static_resources() const;

    ServerConfig& operator=(const ServerConfig& other);

  private:
    void init();

    static constexpr const char s_type[]{"server"};
    StringField m_host{"host", "127.0.0.1"};
    UIntegerField m_port{"port", 8080};
    StringField m_api_prefix{"api_prefix", "api/v1"};

    TypedDictField<WebAppConfig> m_web_app{"web_app"};
    StringField m_backend{"backend", "hestia::Basic"};
    BooleanField m_cache_static_resources{"cache_static"};
    StringField m_static_resource_path{"static_resource_path"};

    StringField m_controller_address{"controller_address"};
    BooleanField m_controller{"is_controller"};
    StringField m_tag{"tag"};
};
}  // namespace hestia