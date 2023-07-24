#include "HsmNode.h"

#include <stdexcept>

namespace hestia {
HsmNode::HsmNode() : OwnableModel(s_model_type, {})
{

    init();
}

HsmNode::HsmNode(const std::string& id) : OwnableModel(id, s_model_type)
{

    init();
}

HsmNode::HsmNode(const HsmNode& other) : OwnableModel(other)
{
    *this = other;
}

HsmNode& HsmNode::operator=(const HsmNode& other)
{
    if (this != &other) {
        OwnableModel::operator=(other);
        m_is_controller = other.m_is_controller;
        m_host_address  = other.m_host_address;
        m_port          = other.m_port;
        m_version       = other.m_version;
        m_app_type      = other.m_app_type;
        m_backends      = other.m_backends;
        init();
    }
    return *this;
}

void HsmNode::init()
{
    register_scalar_field(&m_is_controller);
    register_scalar_field(&m_host_address);
    register_scalar_field(&m_port);
    register_scalar_field(&m_version);
    register_scalar_field(&m_app_type);

    register_sequence_field(&m_backends);
}

void HsmNode::add_backend(const HsmObjectStoreClientBackend& backend)
{
    m_backends.get_container_as_writeable().push_back(backend);
}

std::string HsmNode::get_type()
{
    return s_model_type;
}

void HsmNode::set_backends(
    const std::vector<HsmObjectStoreClientBackend>& backends)
{
    m_backends.get_container_as_writeable() = backends;
}

bool HsmNode::is_controller() const
{
    return m_is_controller.get_value();
}

const std::string& HsmNode::host() const
{
    return m_host_address.get_value();
}

unsigned HsmNode::port() const
{
    return m_port.get_value();
}

const std::vector<HsmObjectStoreClientBackend>& HsmNode::backends() const
{
    return m_backends.container();
}

void HsmNode::set_host_address(const std::string& address)
{
    m_host_address.update_value(address);
}

void HsmNode::set_app_type(const std::string& app_type)
{
    m_app_type.update_value(app_type);
}

void HsmNode::set_port(unsigned port)
{
    m_port.update_value(port);
}

void HsmNode::set_is_controller(bool controller)
{
    m_is_controller.update_value(controller);
}

void HsmNode::set_version(const std::string& version)
{
    m_version.update_value(version);
}

}  // namespace hestia