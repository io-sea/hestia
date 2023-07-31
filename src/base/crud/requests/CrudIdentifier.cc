#include "CrudIdentifier.h"

#include "StringUtils.h"
#include <sstream>

namespace hestia {
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

std::size_t CrudIdentifier::parse(
    const std::string& buffer,
    InputFormat format,
    std::vector<CrudIdentifier>& ids)
{
    if (format == InputFormat::NONE) {
        return 0;
    }

    std::stringstream ss(buffer);
    std::string line;
    std::size_t byte_count{0};
    while (std::getline(ss, line, '\n')) {
        if (line.empty()) {
            byte_count++;
            break;
        }
        CrudIdentifier id;
        id.from_buffer(line, format);
        ids.push_back(id);
        byte_count += line.size() + 1;
    }
    return byte_count;
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

bool CrudIdentifier::has_value() const
{
    return has_parent_primary_key() || has_name();
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
}  // namespace hestia