#pragma once

#include "XmlElement.h"

#include <memory>
#include <vector>

namespace hestia {
class XmlProlog : public XmlElement {
  public:
    XmlProlog(const std::string& tag_name = "xml");

    static std::unique_ptr<XmlProlog> create(
        const std::string& tag_name = "xml");
    using Ptr = std::unique_ptr<XmlProlog>;

    const std::string& get_version() const;
    const std::string& get_encoding() const;

    void set_encoding(const std::string& encoding);
    void set_version(const std::string& version);

    std::string to_string(
        unsigned depth = 0, bool keep_inline = false) const override;

  private:
    std::string m_version;
    std::string m_encoding;
};
}  // namespace hestia
