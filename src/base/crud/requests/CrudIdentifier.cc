#include "CrudIdentifier.h"

#include "StringUtils.h"
#include <iostream>
#include <sstream>

namespace hestia {


CrudIdentifier::CrudIdentifier(const Map& buffer, const FormatSpec& format)
{
    if (const auto name_val = buffer.get_item(format.m_name_key_name);
        !name_val.empty()) {
        m_name = name_val;
    }
    if (const auto parent_name_val =
            buffer.get_item(format.m_parent_name_key_name);
        !parent_name_val.empty()) {
        m_parent_name = parent_name_val;
    }
    if (const auto parent_id_val = buffer.get_item(format.m_parent_id_key_name);
        !parent_id_val.empty()) {
        m_parent_primary_key = parent_id_val;
    }
}

CrudIdentifier::CrudIdentifier(
    const std::string& name, const std::string& parent_key, Type type)
{
    m_name = name;
    if (type == Type::PRIMARY_KEY) {
        m_parent_primary_key = parent_key;
    }
    else {
        m_parent_name = parent_key;
    }
}

CrudIdentifier::CrudIdentifier(
    const std::string& buffer, const FormatSpec& format)
{
    from_buffer(buffer, format.m_input_format);
}

CrudIdentifier::CrudIdentifier(const std::string& key, Type type)
{
    if (type == Type::PRIMARY_KEY) {
        m_primary_key = key;
    }
    else {
        m_name = key;
    }
}

void CrudIdentifier::set_primary_key(const std::string& key)
{
    m_primary_key = key;
}

CrudIdentifier::InputFormat CrudIdentifier::input_format_from_string(
    const std::string& format)
{
    return InputFormat_enum_string_converter().init().from_string(format);
}

void CrudIdentifier::from_buffer(const std::string& line, InputFormat format)
{
    switch (format) {
        case InputFormat::NONE:
            return;
        case InputFormat::ID:
            set_primary_key(line);
            return;
        case InputFormat::ID_PARENT_ID: {
            auto [id_str, parent_id] = StringUtils::split_on_first(line, ',');
            StringUtils::trim(id_str);
            StringUtils::trim(parent_id);
            set_primary_key(id_str);
            set_parent_primary_key(parent_id);
            return;
        }
        case InputFormat::ID_PARENT_NAME: {
            auto [id_str, parent_name] = StringUtils::split_on_first(line, ',');
            StringUtils::trim(id_str);
            StringUtils::trim(parent_name);
            set_primary_key(id_str);
            set_parent_name(parent_name);
            return;
        }
        case InputFormat::NAME:
            set_name(line);
            return;
        case InputFormat::PARENT_NAME:
            set_parent_name(line);
            return;
        case InputFormat::PARENT_ID:
            set_parent_primary_key(line);
            return;
        case InputFormat::NAME_PARENT_ID: {
            auto [name, parent_id] = StringUtils::split_on_first(line, ',');
            StringUtils::trim(name);
            StringUtils::trim(parent_id);
            set_name(name);
            set_parent_primary_key(parent_id);
            return;
        }
        case InputFormat::NAME_PARENT_NAME: {
            auto [name, parent_name] = StringUtils::split_on_first(line, ',');
            StringUtils::trim(name);
            StringUtils::trim(parent_name);
            set_name(name);
            set_parent_name(parent_name);
            return;
        }
        default:
            return;
    }
}

const std::string& CrudIdentifier::get_primary_key() const
{
    return m_primary_key;
}

const std::string& CrudIdentifier::get_name() const
{
    return m_name;
}

const std::string& CrudIdentifier::get_parent_name() const
{
    return m_parent_name;
}

const std::string& CrudIdentifier::get_parent_primary_key() const
{
    return m_parent_primary_key;
}

bool CrudIdentifier::has_value() const
{
    return has_parent_primary_key() || has_name() || has_primary_key()
           || has_parent_name();
}

bool CrudIdentifier::has_primary_key() const
{
    return !m_primary_key.empty();
}

bool CrudIdentifier::has_name() const
{
    return !m_name.empty();
}

bool CrudIdentifier::has_parent_primary_key() const
{
    return !m_parent_primary_key.empty();
}

bool CrudIdentifier::has_parent_name() const
{
    return !m_parent_name.empty();
}

void CrudIdentifier::write(Dictionary& dict, const FormatSpec& format) const
{
    Map items;
    if (!m_primary_key.empty()) {
        items.set_item(format.m_primary_key_name, m_primary_key);
    }
    if (!m_parent_name.empty()) {
        items.set_item(format.m_parent_name_key_name, m_parent_name);
    }
    if (!m_name.empty()) {
        items.set_item(format.m_name_key_name, m_name);
    }
    if (!m_parent_primary_key.empty()) {
        items.set_item(format.m_parent_id_key_name, m_parent_primary_key);
    }
    dict.set_map(items.data());
}

}  // namespace hestia