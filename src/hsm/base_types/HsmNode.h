#pragma once

#include "EnumUtils.h"
#include "ObjectStoreBackend.h"
#include "OwnableModel.h"
#include "SerializeableWithFields.h"

namespace hestia {

class HsmNodeInterface : public SerializeableWithFields {
  public:
    STRINGABLE_ENUM(Type, S3, HTTP)

    HsmNodeInterface();

    HsmNodeInterface(const HsmNodeInterface& other);

    unsigned get_port() const;

    Type get_type() const;

    std::string get_type_as_string() const;

    bool is_auth_enabled() const;

    void set_port(unsigned port);

    HsmNodeInterface& operator=(const HsmNodeInterface& other);

  private:
    void init();

    UIntegerField m_port{"port", 0};
    BooleanField m_enable_auth{"enable_auth", true};
    EnumField<Type, Type_enum_string_converter> m_type{"type", Type::HTTP};
};

class HsmNode : public HsmItem, public OwnableModel {
  public:
    HsmNode();

    HsmNode(const std::string& id);

    HsmNode(const HsmNode& other);

    using Ptr = std::unique_ptr<HsmNode>;

    void add_interface(const HsmNodeInterface& interface);

    const std::vector<ObjectStoreBackend>& backends() const;

    const std::vector<HsmNodeInterface>& get_interfaces() const;

    static std::string get_type();

    const std::string& host() const;

    bool is_controller() const;

    void set_host_address(const std::string& address);

    void set_is_controller(bool controller);

    void set_interfaces(const std::vector<HsmNodeInterface>& interfaces);

    void set_version(const std::string& version);

    HsmNode& operator=(const HsmNode& other);

  private:
    void init();

    static constexpr const char s_model_type[]{"node"};
    BooleanField m_is_controller{"is_controller", false};
    StringField m_host_address{"host_address"};
    StringField m_version{"version"};

    SequenceField<std::vector<HsmNodeInterface>> m_interfaces{"interfaces"};
    ForeignKeyProxyField<ObjectStoreBackend> m_backends{"backends"};
};
}  // namespace hestia