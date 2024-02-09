#pragma once

#include "HsmNode.h"

namespace hestia {

class ServerConfig : public SerializeableWithFields {

  public:
    ServerConfig();

    ServerConfig(const ServerConfig& other);

    const std::string& get_static_resource_path() const;

    const std::string& get_host_address() const;

    unsigned get_port() const;

    const std::string& get_tag() const;

    const std::vector<HsmNodeInterface>& get_interfaces() const;

    static std::string get_type();

    const std::string& get_controller_address() const;

    const std::string& get_access_control_origin() const
    {
        return m_access_control_origin.get_value();
    }

    const std::string& get_api_prefix() const;

    const Map& get_host_mapping() const;

    bool has_controller_address() const;

    bool is_controller() const;

    bool should_block_on_launch() const;

    void set_controller_address(const std::string& host, unsigned port);

    bool should_cache_static_resources() const;

    ServerConfig& operator=(const ServerConfig& other);

  private:
    void init();

    static constexpr const char s_type[]{"server"};
    StringField m_host{"host", "127.0.0.1"};
    UIntegerField m_port{"port", 8080};
    StringField m_api_prefix{"api_prefix", "api/v1"};
    StringField m_access_control_origin{"access_control_origin"};

    SequenceField<std::vector<HsmNodeInterface>> m_interfaces{"interfaces"};

    BooleanField m_cache_static_resources{"cache_static"};
    StringField m_static_resource_path{"static_resource_path"};
    ScalarMapField m_host_mapping{"host_mapping"};

    StringField m_controller_address{"controller_address"};
    StringField m_tag{"tag"};
    BooleanField m_run_blocking{"run_blocking", true};
};
}  // namespace hestia