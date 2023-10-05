#include "S3Owner.h"

#include "XmlElement.h"

namespace hestia {
S3Owner::S3Owner(const XmlElement& element)
{
    for (const auto& child : element.get_children()) {
        if (child->get_tag_name() == "DisplayName") {
            m_display_name = child->get_text();
        }
        else if (child->get_tag_name() == "ID") {
            m_id = child->get_text();
        }
    }
}

XmlElementPtr S3Owner::to_xml() const
{
    auto root = XmlElement::create("Owner");
    if (!m_display_name.empty()) {
        auto e = XmlElement::create("DisplayName");
        e->set_text(m_display_name);
        root->add_child(std::move(e));
    }
    if (!m_id.empty()) {
        auto e = XmlElement::create("ID");
        e->set_text(m_id);
        root->add_child(std::move(e));
    }
    return root;
}

}  // namespace hestia