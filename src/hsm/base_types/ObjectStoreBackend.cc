#include "ObjectStoreBackend.h"

#include <iostream>

namespace hestia {

ObjectStoreBackend::ObjectStoreBackend() :
    HsmItem(HsmItem::Type::OBJECT_STORE_BACKEND),
    Model(HsmItem::object_store_backend_name)
{
    init();
}

ObjectStoreBackend::ObjectStoreBackend(ObjectStoreBackend::Type client_type) :
    HsmItem(HsmItem::Type::OBJECT_STORE_BACKEND),
    Model(HsmItem::object_store_backend_name)
{
    m_backend_type.update_value(client_type);
    init();
}

ObjectStoreBackend::ObjectStoreBackend(const ObjectStoreBackend& other) :
    HsmItem(HsmItem::Type::OBJECT_STORE_BACKEND), Model(other)
{
    *this = other;
}

ObjectStoreBackend& ObjectStoreBackend::operator=(
    const ObjectStoreBackend& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_backend_type      = other.m_backend_type;
        m_config            = other.m_config;
        m_plugin_path       = other.m_plugin_path;
        m_custom_identifier = other.m_custom_identifier;
        m_node              = other.m_node;
        m_tiers             = other.m_tiers;
        init();
    }
    return *this;
}

void ObjectStoreBackend::init()
{
    register_scalar_field(&m_backend_type);
    register_scalar_field(&m_plugin_path);
    register_map_field(&m_config);

    register_scalar_field(&m_custom_identifier);

    register_foreign_key_field(&m_node);
    register_many_to_many_field(&m_tiers);
}

const Dictionary& ObjectStoreBackend::get_config() const
{
    return m_config.value();
}

std::string ObjectStoreBackend::get_type()
{
    return HsmItem::object_store_backend_name;
}

const std::string& ObjectStoreBackend::get_node_id() const
{
    return m_node.get_id();
}

ObjectStoreBackend::Type ObjectStoreBackend::get_backend() const
{
    return m_backend_type.get_value();
}

std::string ObjectStoreBackend::get_backend_as_string() const
{
    return Type_enum_string_converter().init().to_string(
        m_backend_type.get_value());
}

bool ObjectStoreBackend::has_tier(const std::string& tier_id) const
{
    for (const auto& id : m_tiers.get_ids()) {
        if (tier_id == id) {
            return true;
        }
    }
    return false;
}

void ObjectStoreBackend::set_node_id(const std::string& id)
{
    m_node.set_id(id);
}

void ObjectStoreBackend::add_tier_id(const std::string& id)
{
    m_tiers.add_id(id);
}

void ObjectStoreBackend::set_tier_ids(const std::vector<std::string>& ids)
{
    m_tiers.set_ids(ids);
}

const std::vector<std::string> ObjectStoreBackend::get_tier_ids() const
{
    return m_tiers.get_ids();
}

bool ObjectStoreBackend::is_hsm() const
{
    return m_backend_type.get_value() == ObjectStoreBackend::Type::MOTR
           || m_backend_type.get_value() == ObjectStoreBackend::Type::FILE_HSM
           || m_backend_type.get_value() == ObjectStoreBackend::Type::CUSTOM_HSM
           || m_backend_type.get_value()
                  == ObjectStoreBackend::Type::MEMORY_HSM;
}

bool ObjectStoreBackend::is_plugin() const
{
    return m_backend_type.get_value() == ObjectStoreBackend::Type::MOTR
           || m_backend_type.get_value() == ObjectStoreBackend::Type::PHOBOS
           || m_backend_type.get_value() == ObjectStoreBackend::Type::CUSTOM
           || m_backend_type.get_value() == ObjectStoreBackend::Type::MOCK_MOTR
           || m_backend_type.get_value()
                  == ObjectStoreBackend::Type::MOCK_PHOBOS;
}

bool ObjectStoreBackend::is_mock() const
{
    return m_backend_type.get_value() == ObjectStoreBackend::Type::MOCK_MOTR
           || m_backend_type.get_value()
                  == ObjectStoreBackend::Type::MOCK_PHOBOS;
}

std::string ObjectStoreBackend::get_plugin_path() const
{
    if (!m_plugin_path.get_value().empty()) {
        return m_plugin_path.get_value();
    }
    else {
        switch (m_backend_type.get_value()) {
            case ObjectStoreBackend::Type::PHOBOS:
                return "libhestia_phobos_plugin";
            case ObjectStoreBackend::Type::MOTR:
                return "libhestia_motr_plugin";
            case ObjectStoreBackend::Type::MOCK_PHOBOS:
                return "libhestia_mock_phobos_plugin";
            case ObjectStoreBackend::Type::MOCK_MOTR:
                return "libhestia_mock_motr_plugin";
            case ObjectStoreBackend::Type::S3:
            case ObjectStoreBackend::Type::FILE:
            case ObjectStoreBackend::Type::MEMORY:
            case ObjectStoreBackend::Type::FILE_HSM:
            case ObjectStoreBackend::Type::MEMORY_HSM:
            case ObjectStoreBackend::Type::CUSTOM:
            case ObjectStoreBackend::Type::CUSTOM_HSM:
            case ObjectStoreBackend::Type::UNKNOWN:
            default:
                return {};
        }
    }
}

bool ObjectStoreBackend::is_built_in() const
{
    return !is_plugin();
}

std::string ObjectStoreBackend::to_string() const
{
    Type_enum_string_converter type_converter;
    type_converter.init();
    return "Type: " + type_converter.to_string(m_backend_type.get_value());
}
}  // namespace hestia