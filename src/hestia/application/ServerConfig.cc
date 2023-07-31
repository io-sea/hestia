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

        m_web_app                = other.m_web_app;
        m_backend                = other.m_backend;
        m_cache_static_resources = other.m_cache_static_resources;
        m_static_resource_path   = other.m_static_resource_path;

        m_controller_address = other.m_controller_address;
        m_controller         = other.m_controller;
        m_tag                = other.m_tag;
        m_api_prefix         = other.m_api_prefix;
        init();
    }
    return *this;
}

void ServerConfig::init()
{
    register_scalar_field(&m_host);
    register_scalar_field(&m_port);

    register_map_field(&m_web_app);
    register_scalar_field(&m_backend);
    register_scalar_field(&m_cache_static_resources);
    register_scalar_field(&m_static_resource_path);

    register_scalar_field(&m_controller_address);
    register_scalar_field(&m_controller);
    register_scalar_field(&m_api_prefix);
    register_scalar_field(&m_tag);
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

const WebAppConfig& ServerConfig::get_web_app_config() const
{
    return m_web_app.value();
}

const std::string& ServerConfig::get_controller_address() const
{
    return m_controller_address.get_value();
}

bool ServerConfig::has_controller_address() const
{
    return !m_controller_address.get_value().empty();
}

bool ServerConfig::is_controller() const
{
    return m_controller.get_value();
}

bool ServerConfig::should_cache_static_resources() const
{
    return m_cache_static_resources.get_value();
}

}  // namespace hestia