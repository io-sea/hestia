#include "CrudAttributes.h"

namespace hestia {
CrudAttributes::CrudAttributes(const std::string& buffer, Format format) :
    m_buffer(buffer), m_input_format(format)
{
}

const std::string& CrudAttributes::get_buffer() const
{
    return m_buffer;
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
}  // namespace hestia