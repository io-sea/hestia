#pragma once

#include <memory>
#include <string>

namespace hestia {

class XmlElement;
using XmlElementPtr = std::unique_ptr<XmlElement>;

struct S3Owner {
    S3Owner() = default;
    S3Owner(const XmlElement& element);

    XmlElementPtr to_xml() const;

    bool populated() const { return !m_id.empty(); }

    std::string m_display_name;
    std::string m_id;
};
}  // namespace hestia