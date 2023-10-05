#include "XmlElement.h"

#include "XmlAttribute.h"

namespace hestia {
XmlElement::XmlElement(const std::string& tag_name) :
    m_tag_name(tag_name), m_children()
{
}

XmlElement::~XmlElement() {}

XmlElement::Ptr XmlElement::create(const std::string& tag_name)
{
    return std::make_unique<XmlElement>(tag_name);
}

void XmlElement::set_tag_name(const std::string& tag_name)
{
    m_tag_name = tag_name;
}

void XmlElement::add_child(XmlElement::Ptr child)
{
    m_children.push_back(std::move(child));
}

void XmlElement::add_attribute(XmlAttributePtr attribute)
{
    m_attributes[attribute->get_name()] = std::move(attribute);
}

void XmlElement::add_attribute(
    const std::string& name, const std::string& value)
{
    auto attr = std::make_unique<XmlAttribute>(name);
    attr->set_value(value);
    add_attribute(std::move(attr));
}

const std::string& XmlElement::get_tag_name() const
{
    return m_tag_name;
}

const std::string& XmlElement::get_text() const
{
    return m_text;
}

void XmlElement::set_text(const std::string& text)
{
    m_text = text;
}

XmlElement* XmlElement::get_first_child_with_tag_name(const std::string& tag)
{
    for (auto& child : m_children) {
        if (child->get_tag_name() == tag) {
            return child.get();
        }
    }

    return nullptr;
}

bool XmlElement::has_attribute(const std::string& attribute) const
{
    return (bool)(get_attribute(attribute));
}

XmlAttribute* XmlElement::get_attribute(const std::string& attribute_name) const
{
    if (auto iter = m_attributes.find(attribute_name);
        iter != m_attributes.end()) {
        return iter->second.get();
    }
    return nullptr;
}

const std::unordered_map<std::string, XmlAttributePtr>&
XmlElement::get_attributes() const
{
    return m_attributes;
}

const std::vector<std::unique_ptr<XmlElement>>& XmlElement::get_children() const
{
    return m_children;
}

std::string XmlElement::to_string(unsigned depth, bool keep_inline) const
{
    const auto prefix = std::string(2 * depth, ' ');

    std::string line_ending = keep_inline ? "" : "\n";

    auto content = prefix + "<" + get_tag_name();
    for (const auto& [key, attribute] : get_attributes()) {
        content +=
            " " + attribute->get_name() + "=\"" + attribute->get_value() + "\"";
    }

    const auto num_children = m_children.size();
    if (num_children == 0 && get_text().empty()) {
        content += "/>" + line_ending;
        return content;
    }
    else {
        content += ">";
    }

    if (!get_text().empty()) {
        content += get_text();
    }

    if (num_children > 0) {
        content += line_ending;
    }

    for (const auto& child : m_children) {
        content += child->to_string(depth + 1, keep_inline);
    }
    if (num_children > 0) {
        content += prefix;
    }

    content += "</" + get_tag_name() + ">" + line_ending;
    return content;
}
}  // namespace hestia