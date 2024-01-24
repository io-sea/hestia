#include "HsmNode.h"

#include <stdexcept>

namespace hestia {

HsmNodeInterface::HsmNodeInterface() :
    SerializeableWithFields("hsm_node_interface")
{
    init();
}

void HsmNodeInterface::init()
{
    register_scalar_field(&m_port);
    register_scalar_field(&m_type);
    register_scalar_field(&m_enable_auth);
}

HsmNodeInterface::HsmNodeInterface(const HsmNodeInterface& other) :
    SerializeableWithFields("hsm_node_interface")
{
    *this = other;
}

HsmNodeInterface& HsmNodeInterface::operator=(const HsmNodeInterface& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_port        = other.m_port;
        m_type        = other.m_type;
        m_enable_auth = other.m_enable_auth;
        init();
    }
    return *this;
}

unsigned HsmNodeInterface::get_port() const
{
    return m_port.get_value();
}

void HsmNodeInterface::set_port(unsigned port)
{
    m_port.update_value(port);
}

bool HsmNodeInterface::is_auth_enabled() const
{
    return m_enable_auth.get_value();
}

HsmNodeInterface::Type HsmNodeInterface::get_type() const
{
    return m_type.get_value();
}

std::string HsmNodeInterface::get_type_as_string() const
{
    return Type_enum_string_converter().init().to_string(get_type());
}

HsmNode::HsmNode() :
    HsmItem(HsmItem::Type::NODE), OwnableModel(s_model_type, {})
{
    init();
}

HsmNode::HsmNode(const std::string& id) :
    HsmItem(HsmItem::Type::NODE), OwnableModel(id, s_model_type)
{
    init();
}

HsmNode::HsmNode(const HsmNode& other) :
    HsmItem(HsmItem::Type::NODE), OwnableModel(other)
{
    *this = other;
}

HsmNode& HsmNode::operator=(const HsmNode& other)
{
    if (this != &other) {
        OwnableModel::operator=(other);
        m_is_controller = other.m_is_controller;
        m_host_address  = other.m_host_address;
        m_interfaces    = other.m_interfaces;
        m_version       = other.m_version;
        m_backends      = other.m_backends;
        init();
    }
    return *this;
}

void HsmNode::init()
{
    m_name.set_index_scope(BaseField::IndexScope::GLOBAL);

    register_scalar_field(&m_is_controller);
    register_scalar_field(&m_host_address);
    register_sequence_field(&m_interfaces);
    register_scalar_field(&m_version);
    register_foreign_key_proxy_field(&m_backends);
}

std::string HsmNode::get_type()
{
    return s_model_type;
}

const std::vector<HsmNodeInterface>& HsmNode::get_interfaces() const
{
    return m_interfaces.container();
}

bool HsmNode::is_controller() const
{
    return m_is_controller.get_value();
}

const std::string& HsmNode::host() const
{
    return m_host_address.get_value();
}

const std::vector<ObjectStoreBackend>& HsmNode::backends() const
{
    return m_backends.models();
}

void HsmNode::set_host_address(const std::string& address)
{
    m_host_address.update_value(address);
}

void HsmNode::add_interface(const HsmNodeInterface& interface)
{
    m_interfaces.get_container_as_writeable().push_back(interface);
}

void HsmNode::set_is_controller(bool controller)
{
    m_is_controller.update_value(controller);
}

void HsmNode::set_interfaces(const std::vector<HsmNodeInterface>& interfaces)
{
    m_interfaces.get_container_as_writeable() = interfaces;
}

void HsmNode::set_version(const std::string& version)
{
    m_version.update_value(version);
}

}  // namespace hestia