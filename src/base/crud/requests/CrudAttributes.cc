#include "CrudAttributes.h"

namespace hestia {
CrudAttributes::CrudAttributes(const std::string& buffer, Format format) :
    m_buffer(buffer), m_input_format(format)
{
}

CrudAttributes::CrudAttributes(const Map& buffer, Format format) :
    m_map_buffer(buffer), m_input_format(format)
{
}

const std::string& CrudAttributes::get_buffer() const
{
    return m_buffer;
}

std::string& CrudAttributes::buffer()
{
    return m_buffer;
}

const Map& CrudAttributes::get_map_buffer() const
{
    return m_map_buffer;
}

CrudAttributes::Format CrudAttributes::get_input_format() const
{
    return m_input_format;
}

CrudAttributes::Format CrudAttributes::get_output_format() const
{
    return m_output_format;
}

std::string CrudAttributes::get_output_format_as_string() const
{
    return to_string(m_output_format);
}

std::string CrudAttributes::get_input_format_as_string() const
{
    return to_string(m_input_format);
}

bool CrudAttributes::is_json_input() const
{
    return m_input_format == Format::JSON;
}

CrudAttributes::Format CrudAttributes::format_from_string(
    const std::string& format)
{
    return Format_enum_string_converter().init().from_string(format);
}

bool CrudAttributes::has_content() const
{
    return !m_buffer.empty();
}

const std::string& CrudAttributes::get_key_prefix() const
{
    return m_key_prefix;
}

void CrudAttributes::set_buffer(const std::string& buffer, Format format)
{
    m_input_format = format;
    m_buffer       = buffer;
}

void CrudAttributes::set_key_prefix(const std::string& prefix)
{
    m_key_prefix = prefix;
}

void CrudAttributes::set_input_format(Format format)
{
    m_input_format = format;
}

void CrudAttributes::set_output_format(Format format)
{
    m_output_format = format;
}

std::string CrudAttributes::to_string(Format format)
{
    switch (format) {
        case Format::JSON:
            return "json";
        case Format::KEY_VALUE:
            return "key_value";
        case Format::MAP:
            return "map";
        case Format::NONE:
            return "none";
    }
    return "none";
}

}  // namespace hestia