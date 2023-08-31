#include "LoggerConfig.h"

namespace hestia {
LoggerConfig::LoggerConfig() : SerializeableWithFields(s_type)
{
    init();
}

LoggerConfig::LoggerConfig(const LoggerConfig& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

LoggerConfig& LoggerConfig::operator=(const LoggerConfig& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_log_file_path   = other.m_log_file_path;
        m_log_file_prefix = other.m_log_file_prefix;
        m_active          = other.m_active;
        m_console_only    = other.m_console_only;
        m_syslog_only     = other.m_syslog_only;
        m_assert          = other.m_assert;
        m_level           = other.m_level;
        init();
    }
    return *this;
}

void LoggerConfig::init()
{
    register_scalar_field(&m_log_file_path);
    register_scalar_field(&m_log_file_prefix);
    register_scalar_field(&m_active);
    register_scalar_field(&m_console_only);
    register_scalar_field(&m_syslog_only);
    register_scalar_field(&m_assert);
    register_scalar_field(&m_level);
}

const std::string& LoggerConfig::get_log_path() const
{
    return m_log_file_path.get_value();
}

const std::string& LoggerConfig::get_log_prefix() const
{
    return m_log_file_prefix.get_value();
}

bool LoggerConfig::is_active() const
{
    return m_active.get_value();
}

bool LoggerConfig::is_console_only() const
{
    return m_console_only.get_value();
}

bool LoggerConfig::is_syslog_only() const
{
    return m_syslog_only.get_value();
}

std::string LoggerConfig::get_type()
{
    return s_type;
}
}  // namespace hestia