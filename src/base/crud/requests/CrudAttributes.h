#pragma once

#include "EnumUtils.h"
#include "Map.h"

namespace hestia {
class CrudAttributes {

  public:
    STRINGABLE_ENUM(Format, NONE, JSON, KEY_VALUE)

    CrudAttributes() = default;

    CrudAttributes(const std::string& buffer, Format format = Format::JSON);

    const std::string& get_buffer() const;

    std::string& buffer() { return m_buffer; }

    bool is_json_input() const;

    bool has_content() const;

    Format get_input_format() const { return m_input_format; }

    Format get_output_format() const { return m_output_format; }

    std::string get_output_format_as_string() const
    {
        return to_string(m_output_format);
    }

    std::string get_input_format_as_string() const
    {
        return to_string(m_input_format);
    }

    const std::string& get_key_prefix() const { return m_key_prefix; }

    static Format format_from_string(const std::string& format);

    void set_buffer(const std::string& buffer, Format format = Format::JSON)
    {
        m_input_format = format;
        m_buffer       = buffer;
    }

    void set_key_prefix(const std::string& prefix) { m_key_prefix = prefix; }

    void set_input_format(Format format) { m_input_format = format; }

    void set_output_format(Format format) { m_output_format = format; }

    static std::string to_string(Format format)
    {
        return format == Format::JSON ? "json" : "key_value";
    }

  private:
    std::string m_buffer;
    std::string m_key_prefix;
    Format m_input_format{Format::JSON};
    Format m_output_format{Format::JSON};
};
}  // namespace hestia