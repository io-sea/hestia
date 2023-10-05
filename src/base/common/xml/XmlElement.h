#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace hestia {
class XmlAttribute;
using XmlAttributePtr = std::unique_ptr<XmlAttribute>;

class XmlElement {
  public:
    XmlElement(const std::string& tag_name);
    virtual ~XmlElement();

    using Ptr = std::unique_ptr<XmlElement>;

    static std::unique_ptr<XmlElement> create(const std::string& tag_name);

    void add_attribute(XmlAttributePtr attribute);
    void add_attribute(const std::string& name, const std::string& value);
    void add_child(std::unique_ptr<XmlElement> child);

    const std::string& get_tag_name() const;
    const std::string& get_text() const;

    bool has_attribute(const std::string& attribute) const;
    XmlAttribute* get_attribute(const std::string& attribute) const;
    const std::unordered_map<std::string, XmlAttributePtr>& get_attributes()
        const;

    const std::vector<std::unique_ptr<XmlElement>>& get_children() const;

    XmlElement* get_first_child_with_tag_name(const std::string& tag);

    void set_text(const std::string& text);
    void set_tag_name(const std::string& tag_name);

    virtual std::string to_string(
        unsigned depth = 0, bool keep_inline = false) const;

  protected:
    std::string m_tag_name;
    std::string m_text;

    std::unordered_map<std::string, XmlAttributePtr> m_attributes;
    std::vector<std::unique_ptr<XmlElement>> m_children;
};
}  // namespace hestia
