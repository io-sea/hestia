#pragma once

#include "EnumUtils.h"
#include "Map.h"

namespace hestia {
class CrudAttributes {

  public:
    STRINGABLE_ENUM(Format, NONE, JSON, KEY_VALUE, MAP)

    CrudAttributes() = default;

    CrudAttributes(const std::string& buffer, Format format = Format::JSON);

    CrudAttributes(const Map& buffer, Format format = Format::MAP);

    std::string& buffer();

    static Format format_from_string(const std::string& format);

    const std::string& get_buffer() const;

    const Map& get_map_buffer() const;

    Format get_input_format() const;

    Format get_output_format() const;

    std::string get_output_format_as_string() const;

    std::string get_input_format_as_string() const;

    const std::string& get_key_prefix() const;

    bool is_json_input() const;

    bool has_content() const;

    void set_buffer(const std::string& buffer, Format format = Format::JSON);

    void set_key_prefix(const std::string& prefix);

    void set_input_format(Format format);

    void set_output_format(Format format);

    static std::string to_string(Format format);

  private:
    Map m_map_buffer;
    std::string m_buffer;
    std::string m_key_prefix;
    Format m_input_format{Format::JSON};
    Format m_output_format{Format::JSON};
};
}  // namespace hestia