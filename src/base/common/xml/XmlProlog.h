#pragma once

#include "XmlElement.h"

#include <memory>
#include <vector>

namespace hestia {
class XmlProlog : public XmlElement {
  public:
    enum class Version { V1_0 };

    enum class Encoding { UTF8 };

  public:
    XmlProlog(const std::string& tag_name);

    static std::unique_ptr<XmlProlog> create(const std::string& tag_name);
    using Ptr = std::unique_ptr<XmlProlog>;

    Encoding get_encoding() const;
    Version get_version() const;

    void set_encoding(const std::string& encoding);
    void set_version(const std::string& version);
    void update();

  private:
    Version m_version;
    Encoding m_encoding;
};
}  // namespace hestia
