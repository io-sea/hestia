#pragma once

#include <memory>
#include <string>

namespace hestia {
class XmlAttribute {
  public:
    XmlAttribute(const std::string& name, const std::string& value = {});

    static std::unique_ptr<XmlAttribute> create(
        const std::string& name, const std::string& value = {});

    using Ptr = std::unique_ptr<XmlAttribute>;

    const std::string& get_name() const;

    const std::string& get_value() const;

    void set_value(const std::string& value);

  private:
    std::string m_name;
    std::string m_value;
};
}  // namespace hestia