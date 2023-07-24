#pragma once

#include "EnumUtils.h"
#include "SerializeableWithFields.h"

namespace hestia {

class WebAppConfig : public SerializeableWithFields {
  public:
    STRINGABLE_ENUM(Interface, S3, HTTP)

    WebAppConfig();

    WebAppConfig(const WebAppConfig& other);

    const std::string& get_identifier() const;

    Interface get_interface() const;

    std::string get_interface_as_string() const;

    WebAppConfig& operator=(const WebAppConfig& other);

  private:
    void init();

    StringField m_identifier{"hestia::HsmService"};
    EnumField<Interface, Interface_enum_string_converter> m_interface{
        "interface", Interface::HTTP};
};

}  // namespace hestia