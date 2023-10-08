#include "XmlProlog.h"

#include "XmlAttribute.h"

namespace hestia {
XmlProlog::XmlProlog(const std::string& tag_name) :
    XmlElement(tag_name), m_version("1.0"), m_encoding("UTF-8")
{
}

XmlProlog::Ptr XmlProlog::create(const std::string& tag_name)
{
    return std::make_unique<XmlProlog>(tag_name);
}

const std::string& XmlProlog::get_encoding() const
{
    return m_encoding;
}

const std::string& XmlProlog::get_version() const
{
    return m_version;
}

void XmlProlog::set_encoding(const std::string& encoding)
{
    m_encoding = encoding;
}

void XmlProlog::set_version(const std::string& version)
{
    m_version = version;
}

std::string XmlProlog::to_string(unsigned, bool) const
{
    std::string content = "<?xml version=\"" + m_version;
    content += "\" encoding=\"" + m_encoding + "\"?>";
    return content;
}
}  // namespace hestia