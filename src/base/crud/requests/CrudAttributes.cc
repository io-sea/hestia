#include "CrudAttributes.h"

namespace hestia {
CrudAttributes::CrudAttributes(const std::string& buffer, Format format) :
    m_buffer(buffer), m_format(format)
{
}

const std::string& CrudAttributes::get_buffer() const
{
    return m_buffer;
}

bool CrudAttributes::is_json() const
{
    return m_format == Format::JSON;
}

bool CrudAttributes::has_content() const
{
    return !m_buffer.empty();
}
}  // namespace hestia