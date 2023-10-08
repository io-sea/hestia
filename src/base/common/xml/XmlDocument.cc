#include "XmlDocument.h"

#include "XmlAttribute.h"
#include "XmlElement.h"
#include "XmlProlog.h"

#include <iostream>

namespace hestia {
XmlDocument::XmlDocument() : m_prolog(XmlProlog::create()) {}

XmlDocument::~XmlDocument() {}

XmlDocument::Ptr XmlDocument::create()
{
    return std::make_unique<XmlDocument>();
}

void XmlDocument::set_prolog(XmlPrologPtr prolog)
{
    m_prolog = std::move(prolog);
}

XmlProlog* XmlDocument::get_prolog() const
{
    return m_prolog.get();
}

void XmlDocument::set_root(XmlElementPtr root)
{
    m_root = std::move(root);
}

XmlElement* XmlDocument::get_root() const
{
    return m_root.get();
}

bool XmlDocument::has_root() const
{
    return bool(m_root);
}

std::string XmlDocument::to_string(const XmlElement& element)
{
    std::string content = XmlProlog().to_string() + "\n";
    content += element.to_string();
    return content;
}

std::string XmlDocument::to_string() const
{
    std::string content;
    if (m_prolog) {
        content += m_prolog->to_string() + "\n";
    }
    if (m_root) {
        content += m_root->to_string();
    }
    return content;
}

}  // namespace hestia