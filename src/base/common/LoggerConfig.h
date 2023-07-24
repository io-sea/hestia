#pragma once

#include "EnumUtils.h"
#include "SerializeableWithFields.h"

namespace hestia {
class LoggerConfig : public SerializeableWithFields {
  public:
    STRINGABLE_ENUM(Level, DEBUG, INFO, WARN, ERROR)

    LoggerConfig();

    LoggerConfig(const LoggerConfig& other);

    const std::string& get_log_path() const;

    const std::string& get_log_prefix() const;

    static std::string get_type();

    Level get_level() const { return m_level.get_value(); }

    bool is_active() const;

    bool is_console_only() const;

    bool should_assert() const { return m_assert.get_value(); }

    void set_log_path(const std::string& path)
    {
        m_log_file_path.update_value(path);
    }

    void set_log_prefix(const std::string& prefix)
    {
        m_log_file_prefix.update_value(prefix);
    }

    void set_active(bool active) { m_active.update_value(active); }

    void set_console_only(bool console_only)
    {
        m_console_only.update_value(console_only);
    }

    void set_should_assert_on_error(bool should_assert)
    {
        m_assert.update_value(should_assert);
    }

    void set_level(Level level) { m_level.update_value(level); }

    LoggerConfig& operator=(const LoggerConfig& other);

  private:
    void init();

    static constexpr const char s_type[]{"logger"};
    StringField m_log_file_path{"log_path"};
    StringField m_log_file_prefix{"log_prefix"};
    BooleanField m_active{"active", true};
    BooleanField m_assert{"assert", false};
    BooleanField m_console_only{"console_only", false};
    EnumField<Level, Level_enum_string_converter> m_level{"level", Level::INFO};
};
}  // namespace hestia