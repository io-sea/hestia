#include "ServerConfig.h"

namespace hestia {

ServerConfig::ServerConfig() : SerializeableWithFields(s_type)
{
    init();
}

ServerConfig::ServerConfig(const ServerConfig& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_host = other.m_host;
        m_port = other.m_port;

        m_interfaces             = other.m_interfaces;
        m_cache_static_resources = other.m_cache_static_resources;
        m_static_resource_path   = other.m_static_resource_path;

        m_controller_address = other.m_controller_address;
        m_tag                = other.m_tag;
        m_api_prefix         = other.m_api_prefix;
        m_run_blocking       = other.m_run_blocking;

        m_host_mapping = other.m_host_mapping;
        init();
    }
    return *this;
}

void ServerConfig::init()
{
    register_scalar_field(&m_host);
    register_scalar_field(&m_port);

    register_sequence_field(&m_interfaces);
    register_scalar_field(&m_cache_static_resources);
    register_scalar_field(&m_static_resource_path);

    register_scalar_field(&m_controller_address);
    register_scalar_field(&m_api_prefix);
    register_scalar_field(&m_tag);
    register_scalar_field(&m_run_blocking);

    register_map_field(&m_host_mapping);
}

const std::string& ServerConfig::get_tag() const
{
    return m_tag.get_value();
}

std::string ServerConfig::get_type()
{
    return s_type;
}

const std::string& ServerConfig::get_static_resource_path() const
{
    return m_static_resource_path.get_value();
}

const std::string& ServerConfig::get_api_prefix() const
{
    return m_api_prefix.get_value();
}

const std::string& ServerConfig::get_host_address() const
{
    return m_host.get_value();
}

unsigned ServerConfig::get_port() const
{
    return m_port.get_value();
}

const std::vector<HsmNodeInterface>& ServerConfig::get_interfaces() const
{
    return m_interfaces.container();
}

const std::string& ServerConfig::get_controller_address() const
{
    return m_controller_address.get_value();
}

const Map& ServerConfig::get_host_mapping() const
{
    return m_host_mapping.get_map();
}

bool ServerConfig::has_controller_address() const
{
    return !m_controller_address.get_value().empty();
}

bool ServerConfig::should_block_on_launch() const
{
    return m_run_blocking.get_value();
}

bool ServerConfig::is_controller() const
{
    return has_controller_address();
}

bool ServerConfig::should_cache_static_resources() const
{
    return m_cache_static_resources.get_value();
}

void ServerConfig::set_controller_address(
    const std::string& host, unsigned port)
{
    m_controller_address.update_value(host + ":" + std::to_string(port));
}

}  // namespace hestia