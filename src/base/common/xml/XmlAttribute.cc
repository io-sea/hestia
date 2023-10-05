#include "XmlAttribute.h"

namespace hestia {
XmlAttribute::XmlAttribute(const std::string& name, const std::string& value) :
    m_name(name), m_value(value)
{
}

XmlAttribute::Ptr XmlAttribute::create(
    const std::string& name, const std::string& value)
{
    return std::make_unique<XmlAttribute>(name, value);
}

const std::string& XmlAttribute::get_name() const
{
    return m_name;
}

const std::string& XmlAttribute::get_value() const
{
    return m_value;
}

void XmlAttribute::set_value(const std::string& value)
{
    m_value = value;
}
}  // namespace hestia