#include "HsmObjectStoreClientBackend.h"

#include <iostream>

namespace hestia {

HsmObjectStoreClientBackend::HsmObjectStoreClientBackend() :
    SerializeableWithFields(s_type)
{
    init();
}

HsmObjectStoreClientBackend::HsmObjectStoreClientBackend(
    Type client_type, const std::string& identifier) :
    SerializeableWithFields(s_type)
{
    m_backend_type.update_value(client_type);
    m_identifier.update_value(identifier);
    init();
}

HsmObjectStoreClientBackend::HsmObjectStoreClientBackend(
    const HsmObjectStoreClientBackend& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

HsmObjectStoreClientBackend& HsmObjectStoreClientBackend::operator=(
    const HsmObjectStoreClientBackend& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_backend_type = other.m_backend_type;
        m_identifier   = other.m_identifier;
        m_config       = other.m_config;
        m_plugin_path  = other.m_plugin_path;
        init();
    }
    return *this;
}

void HsmObjectStoreClientBackend::init()
{
    register_scalar_field(&m_backend_type);
    register_scalar_field(&m_identifier);
    register_scalar_field(&m_plugin_path);
    register_map_field(&m_config);
}

const Dictionary& HsmObjectStoreClientBackend::get_config() const
{
    return m_config.value();
}

std::string HsmObjectStoreClientBackend::get_type()
{
    return s_type;
}

bool HsmObjectStoreClientBackend::is_hsm() const
{
    return m_backend_type.get_value() == Type::MOTR
           || m_backend_type.get_value() == Type::FILE_HSM
           || m_backend_type.get_value() == Type::MEMORY_HSM;
}

bool HsmObjectStoreClientBackend::is_plugin() const
{
    return m_backend_type.get_value() == Type::MOTR
           || m_backend_type.get_value() == Type::PHOBOS
           || m_backend_type.get_value() == Type::S3
           || m_backend_type.get_value() == Type::MOCK_MOTR
           || m_backend_type.get_value() == Type::MOCK_PHOBOS
           || m_backend_type.get_value() == Type::MOCK_S3;
}

bool HsmObjectStoreClientBackend::is_mock() const
{
    return m_backend_type.get_value() == Type::MOCK_MOTR
           || m_backend_type.get_value() == Type::MOCK_PHOBOS
           || m_backend_type.get_value() == Type::MOCK_S3;
}

bool HsmObjectStoreClientBackend::is_built_in() const
{
    return !is_plugin();
}

const std::string& HsmObjectStoreClientBackend::get_identifier() const
{
    return m_identifier.get_value();
}

std::string HsmObjectStoreClientBackend::to_string() const
{
    Type_enum_string_converter type_converter;
    type_converter.init();
    return "Type: " + type_converter.to_string(m_backend_type.get_value())
           + " | Identifier: " + m_identifier.get_value();
}
}  // namespace hestia