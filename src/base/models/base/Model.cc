#include "Model.h"

#include "RelationshipField.h"

#include <iostream>
#include <stdexcept>

namespace hestia {

Model::Model(const std::string& type) : SerializeableWithFields(type, true)
{
    init();
}

Model::Model(const std::string& id, const std::string& type) :
    SerializeableWithFields(id, type)
{
    init();
}

Model::Model(const Model& other) : SerializeableWithFields(other)
{
    *this = other;
}

Model& Model::operator=(const Model& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_name               = other.m_name;
        m_creation_time      = other.m_creation_time;
        m_last_modified_time = other.m_last_modified_time;
        m_has_owner          = other.m_has_owner;
        init();
    }
    return *this;
}

void Model::init()
{
    register_scalar_field(&m_name);
    register_scalar_field(&m_creation_time);
    register_scalar_field(&m_last_modified_time);
}

void Model::set_has_owner(bool has_owner)
{
    m_has_owner = has_owner;
}

void Model::register_foreign_key_field(ForeignKeyField* field)
{
    m_map_fields[field->get_name()]         = field;
    m_foreign_key_fields[field->get_name()] = field;
}

void Model::register_many_to_many_field(ManyToManyField* field)
{
    m_map_fields[field->get_name()]          = field;
    m_many_to_many_fields[field->get_name()] = field;
}

void Model::register_foreign_key_proxy_field(DictField* field)
{
    m_sequence_fields[field->get_name()]          = field;
    m_foreign_key_proxy_fields[field->get_name()] = field;
}

void Model::register_one_to_one_proxy_field(DictField* field)
{
    m_map_fields[field->get_name()]              = field;
    m_one_to_one_proxy_fields[field->get_name()] = field;
}

Model::~Model() {}

std::time_t Model::get_last_modified_time() const
{
    return m_last_modified_time.get_value();
}

std::string Model::get_runtime_type() const
{
    return m_type.get_value();
}

std::time_t Model::get_creation_time() const
{
    return m_creation_time.get_value();
}

std::string Model::get_parent_type() const
{
    if (m_foreign_key_fields.empty()) {
        return {};
    }
    if (m_foreign_key_fields.size() == 1) {
        return m_foreign_key_fields.begin()->second->get_runtime_type();
    }
    for (const auto& [key, field] : m_foreign_key_fields) {
        if (field->is_parent()) {
            return field->get_runtime_type();
        }
    }
    throw std::runtime_error(
        "Ambigious request for parent type in model type: "
        + get_runtime_type());
}

std::string Model::get_child_id_by_type(const std::string& type) const
{
    for (const auto& field : m_one_to_one_proxy_fields) {
        if (field.second->get_runtime_type() == type) {
            Dictionary field_dict;
            field.second->serialize(field_dict);
            if (field_dict.has_map_item("id")) {
                return field_dict.get_map_item("id")->get_scalar();
            }
        }
    }

    for (const auto& field : m_foreign_key_proxy_fields) {
        if (field.second->get_runtime_type() == type) {
            Dictionary field_dict;
            field.second->serialize(field_dict);
            if (field_dict.has_map_item("id")) {
                return field_dict.get_map_item("id")->get_scalar();
            }
        }
    }
    return {};
}

std::string Model::get_parent_id() const
{
    if (m_foreign_key_fields.empty()) {
        return {};
    }
    if (m_foreign_key_fields.size() == 1) {
        return m_foreign_key_fields.begin()->second->get_id();
    }
    for (const auto& [key, field] : m_foreign_key_fields) {
        if (field->is_parent()) {
            return field->get_id();
        }
    }
    throw std::runtime_error(
        "Ambigious request for parent type in model type: "
        + get_runtime_type());
}

bool Model::has_owner() const
{
    return m_has_owner;
}

void Model::init_creation_time(std::time_t ctime)
{
    m_creation_time.init_value(ctime);
}

void Model::init_modification_time(std::time_t mtime)
{
    m_last_modified_time.init_value(mtime);
}

const std::string& Model::name() const
{
    return m_name.get_value();
}

void Model::set_name(const std::string& name)
{
    m_name.update_value(name);
}

void Model::set_type(const std::string& type)
{
    m_type.update_value(type);
}

bool Model::valid() const
{
    return !m_id.get_value().empty();
}

void Model::get_foreign_key_fields(VecForeignKeyContext& fields) const
{
    for (const auto& [name, dict] : m_foreign_key_fields) {
        fields.push_back(
            {dict->get_runtime_type(), dict->get_name(), dict->get_id()});
    }
}

void Model::get_many_to_many_fields(std::vector<TypeIdsPair>& fields) const
{
    for (const auto& [name, dict] : m_many_to_many_fields) {
        fields.push_back({dict->get_runtime_type(), dict->get_ids()});
    }
}

void Model::get_foreign_key_proxy_fields(VecKeyValuePair& fields) const
{
    for (const auto& [name, dict] : m_foreign_key_proxy_fields) {
        fields.push_back({dict->get_runtime_type(), dict->get_name()});
    }
}

void Model::get_default_create_one_to_one_fields(VecKeyValuePair& fields) const
{
    for (const auto& [name, dict] : m_one_to_one_proxy_fields) {

        if (auto as_relationship_field = dynamic_cast<RelationshipField*>(dict);
            as_relationship_field != nullptr) {
            if (as_relationship_field->should_default_create()) {
                fields.push_back({dict->get_runtime_type(), dict->get_name()});
            }
        }
    }
}

}  // namespace hestia
