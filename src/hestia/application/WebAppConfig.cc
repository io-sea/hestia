#include "WebAppConfig.h"

namespace hestia {
WebAppConfig::WebAppConfig() : SerializeableWithFields("web_app_config")
{
    init();
}

void WebAppConfig::init()
{
    register_scalar_field(&m_identifier);
    register_scalar_field(&m_interface);
}

WebAppConfig::WebAppConfig(const WebAppConfig& other) :
    SerializeableWithFields("web_app_config")
{
    *this = other;
}

WebAppConfig& WebAppConfig::operator=(const WebAppConfig& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_identifier = other.m_identifier;
        m_interface  = other.m_interface;
        init();
    }
    return *this;
}

const std::string& WebAppConfig::get_identifier() const
{
    return m_identifier.get_value();
}

WebAppConfig::Interface WebAppConfig::get_interface() const
{
    return m_interface.get_value();
}

std::string WebAppConfig::get_interface_as_string() const
{
    return Interface_enum_string_converter().init().to_string(
        m_interface.get_value());
}
}  // namespace hestia
