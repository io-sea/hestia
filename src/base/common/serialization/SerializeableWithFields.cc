#include "SerializeableWithFields.h"

#include <iostream>
#include <stdexcept>

namespace hestia {

SerializeableWithFields::SerializeableWithFields(
    const std::string& type, bool use_id) :
    Serializeable(type), m_use_id(use_id)
{
    m_type.init_value(type);
    init();
}

SerializeableWithFields::SerializeableWithFields(
    const std::string& id, const std::string& type) :
    Serializeable(type), m_use_id(true)
{
    m_id.init_value(id);
    m_type.init_value(type);
    init();
}

SerializeableWithFields::SerializeableWithFields(
    const SerializeableWithFields& other) :
    Serializeable(other)
{
    *this = other;
}

SerializeableWithFields& SerializeableWithFields::operator=(
    const SerializeableWithFields& other)
{
    if (this != &other) {
        m_use_id = other.m_use_id;
        m_id     = other.m_id;
        m_type   = other.m_type;

        init();
    }
    return *this;
}

void SerializeableWithFields::init()
{
    m_scalar_fields.clear();
    m_map_fields.clear();
    m_sequence_fields.clear();

    register_scalar_field(&m_type);
    if (m_use_id) {
        register_scalar_field(&m_id);
    }

    for (const auto& [name, value] : m_scalar_fields) {
        if (value->is_primary_key()) {
            m_primary_key = value;
        }
    }
    if (m_primary_key == nullptr && m_use_id) {
        m_primary_key = &m_id;
    }
}

std::string SerializeableWithFields::get_primary_key() const
{
    if (m_primary_key == nullptr) {
        throw std::runtime_error(
            "Primary key requested but none set on Serializeable");
    }
    return m_primary_key->value_as_string();
}

std::string SerializeableWithFields::get_primary_key_name() const
{
    if (m_primary_key == nullptr) {
        throw std::runtime_error(
            "Primary key requested but none set on Serializeable");
    }
    return m_primary_key->get_name();
}

void SerializeableWithFields::set_primary_key(const std::string& key)
{
    if (m_primary_key == nullptr) {
        throw std::runtime_error(
            "Primary key requested for update but none set on Serializeable");
    }
    m_primary_key->value_from_string(key);
}

const std::string& SerializeableWithFields::id() const
{
    return m_id.get_value();
}

void SerializeableWithFields::init_id(const std::string& id)
{
    m_id.init_value(id);
}

void SerializeableWithFields::get_index_fields(
    std::vector<IndexField>& fields) const
{
    for (const auto& [name, value] : m_scalar_fields) {
        if (value->get_index_scope() != BaseField::IndexScope::NONE
            && !value->value_as_string().empty()) {
            fields.push_back(
                {value->get_index_scope(), name, value->value_as_string()});
        }
    }
}

void SerializeableWithFields::register_scalar_field(ScalarField* field)
{
    m_scalar_fields[field->get_name()] = field;
}

void SerializeableWithFields::register_map_field(DictField* field)
{
    m_map_fields[field->get_name()] = field;
}

void SerializeableWithFields::register_sequence_field(DictField* field)
{
    m_sequence_fields[field->get_name()] = field;
}

bool SerializeableWithFields::modified() const
{
    for (const auto& [field_name, field] : m_scalar_fields) {
        if (field->modified()) {
            return true;
        }
    }

    for (const auto& [field_name, field] : m_sequence_fields) {
        if (field->modified()) {
            return true;
        }
    }

    for (const auto& [field_name, field] : m_map_fields) {
        if (field->modified()) {
            return true;
        }
    }

    return false;
}

void SerializeableWithFields::reset()
{
    for (auto& [field_name, field] : m_scalar_fields) {
        field->reset();
    }

    for (auto& [field_name, field] : m_sequence_fields) {
        field->reset();
    }

    for (auto& [field_name, field] : m_map_fields) {
        field->reset();
    }
}

void SerializeableWithFields::deserialize(const Dictionary& dict, Format format)
{
    auto on_item = [this,
                    format](const std::string& key, const std::string& value) {
        if (key == m_id.get_name() && !value.empty()
            && m_id.get_value().empty()) {
            m_id.value_from_string(value);
        }

        if (format == Format::ID) {
            return;
        }

        for (const auto& [field_name, field] : m_scalar_fields) {
            if (key == field_name && !value.empty()) {
                field->value_from_string(value);
            }
        }
    };
    dict.for_each_scalar(on_item);

    for (const auto& [field_name, field] : m_map_fields) {
        if (dict.has_map_item(field_name)) {
            field->deserialize(
                *dict.get_map_item(field_name),
                format == Format::CHILD_ID ? Format::ID : Format::FULL);
        }
    }

    for (const auto& [field_name, field] : m_sequence_fields) {
        if (dict.has_map_item(field_name)) {
            field->deserialize(
                *dict.get_map_item(field_name),
                format == Format::CHILD_ID ? Format::ID : Format::FULL);
        }
    }
}

Dictionary::ScalarType get_dict_type(const std::string& scalar_type)
{
    if (scalar_type == "int") {
        return Dictionary::ScalarType::INT;
    }
    else if (scalar_type == "bool") {
        return Dictionary::ScalarType::BOOL;
    }
    else {
        return Dictionary::ScalarType::STRING;
    }
}

void SerializeableWithFields::serialize(Dictionary& dict, Format format) const
{
    if (m_use_id) {
        dict.add_scalar_item(
            m_id.get_name(), m_id.value_as_string(),
            get_dict_type(m_id.get_scalar_type()));
    }

    if (format != Format::ID) {
        if (format == Format::FULL) {
            for (const auto& [field_name, field] : m_scalar_fields) {
                const auto field_value = field->value_as_string();
                if (!field_value.empty()) {
                    dict.add_scalar_item(
                        field_name, field_value,
                        get_dict_type(field->get_scalar_type()));
                }
            }
        }
        else {
            for (const auto& [field_name, field] : m_scalar_fields) {
                if (field->modified()) {
                    dict.add_scalar_item(
                        field_name, field->value_as_string(),
                        get_dict_type(field->get_scalar_type()));
                }
            }
        }
    }

    if (format == Format::ID) {
        return;
    }

    for (const auto& [field_name, field] : m_map_fields) {
        if (format == Format::MODIFIED && !field->modified()) {
            continue;
        }

        auto field_dict = std::make_unique<Dictionary>();
        field->serialize(
            *field_dict,
            format == Format::CHILD_ID ? Format::ID : Format::FULL);
        dict.set_map_item(field_name, std::move(field_dict));
    }

    for (const auto& [field_name, field] : m_sequence_fields) {
        if (format == Format::MODIFIED && !field->modified()) {
            continue;
        }

        auto field_dict =
            std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
        field->serialize(
            *field_dict,
            format == Format::CHILD_ID ? Format::ID : Format::FULL);
        dict.set_map_item(field_name, std::move(field_dict));
    }
}

}  // namespace hestia