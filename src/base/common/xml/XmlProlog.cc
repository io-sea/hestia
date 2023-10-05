#include "XmlProlog.h"

#include "XmlAttribute.h"

namespace hestia {
XmlProlog::XmlProlog(const std::string& tag_name) :
    XmlElement(tag_name),
    m_version(XmlProlog::Version::V1_0),
    m_encoding(XmlProlog::Encoding::UTF8)
{
    add_attribute(XmlAttribute::create("encoding", "UTF-8"));
    add_attribute(XmlAttribute::create("version", "1.0"));
}

XmlProlog::Ptr XmlProlog::create(const std::string& tag_name)
{
    return std::make_unique<XmlProlog>(tag_name);
}

XmlProlog::Encoding XmlProlog::get_encoding() const
{
    return m_encoding;
}

XmlProlog::Version XmlProlog::get_version() const
{
    return m_version;
}

void XmlProlog::set_encoding(const std::string& encoding)
{
    if (encoding == "UTF-8") {
        m_encoding = XmlProlog::Encoding::UTF8;
    }
}

void XmlProlog::set_version(const std::string& version)
{
    if (version == "1.0") {
        m_version = XmlProlog::Version::V1_0;
    }
}

void XmlProlog::update()
{
    if (const auto version = get_attribute("version")) {
        set_version(version->get_value());
    }

    if (const auto encoding = get_attribute("encoding")) {
        set_encoding(encoding->get_value());
    }
}
}  // namespace hestia