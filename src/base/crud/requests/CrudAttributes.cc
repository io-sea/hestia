#include "CrudAttributes.h"

#include "JsonUtils.h"

#include <iostream>

namespace hestia {

CrudAttributes::Format CrudAttributes::FormatSpec::from_string(
    const std::string& format)
{
    return Format_enum_string_converter().init().from_string(format);
}

std::string CrudAttributes::FormatSpec::to_string(Format format)
{
    return Format_enum_string_converter().init().to_string(format);
}

bool CrudAttributes::FormatSpec::is_none_type() const
{
    return m_type == Format::NONE;
}

bool CrudAttributes::FormatSpec::is_unset() const
{
    return m_type == Format::UNSET;
}

bool CrudAttributes::FormatSpec::is_json() const
{
    return m_type == Format::JSON;
}

void CrudAttributes::FormatSpec::set_is_dict()
{
    m_type = Format::DICT;
}

void CrudAttributes::FormatSpec::set_is_json()
{
    m_type = Format::JSON;
}

CrudAttributes::CrudAttributes() :
    m_dict(Dictionary::create()), m_json(JsonDocument::create())
{
}

CrudAttributes::CrudAttributes(
    const std::string& body, const FormatSpec& format) :
    CrudAttributes()
{
    append(body, format);
}

CrudAttributes::CrudAttributes(const Map& map, const FormatSpec& format) :
    m_dict(Dictionary::create(map, format.m_dict_format)),
    m_json(JsonDocument::create()),
    m_format(format)
{
    m_format.set_is_dict();
}

CrudAttributes::CrudAttributes(JsonDocument::Ptr json) :
    m_dict(Dictionary::create()), m_json(std::move(json))
{
    m_format.set_is_json();
}

CrudAttributes::CrudAttributes(Dictionary::Ptr dict) :
    m_dict(std::move(dict)), m_json(JsonDocument::create())
{
    m_format.set_is_dict();
}

CrudAttributes::CrudAttributes(const CrudAttributes& other)
{
    m_dict       = std::make_unique<Dictionary>(*other.m_dict);
    m_json       = std::make_unique<JsonDocument>(*other.m_json);
    m_format     = other.m_format;
    m_key_prefix = other.m_key_prefix;
}

void CrudAttributes::append(const std::string& buffer, const FormatSpec& format)
{
    if (m_format.is_unset()) {
        m_format = format;
        if (is_json()) {
            m_json->load(buffer);
        }
        else {
            m_dict->from_string(buffer, m_key_prefix, format.m_dict_format);
        }
    }
    else if (format.is_json()) {
        if (is_json()) {
            m_json->append(buffer);
        }
        else {
            Dictionary working_dict;
            JsonDocument(buffer).write(working_dict);
            m_dict->merge(working_dict, true);
        }
    }
    else {
        if (is_json()) {
            Dictionary dict;
            dict.from_string(buffer, m_key_prefix, format.m_dict_format);
            m_json->append(dict);
        }
        else {
            if (m_dict->is_empty()) {
                m_dict->from_string(buffer, m_key_prefix, format.m_dict_format);
            }
            else {
                m_dict->merge(Dictionary(buffer, format.m_dict_format), true);
            }
        }
    }
}

JsonDocument::Ptr CrudAttributes::get_copy_as_json() const
{
    if (is_json()) {
        return std::make_unique<JsonDocument>(*m_json);
    }
    else {
        return JsonDocument::create(*m_dict);
    }
}

Dictionary::Ptr CrudAttributes::get_copy_as_dict() const
{
    if (is_json()) {
        auto dict = Dictionary::create();
        m_json->write(*dict);
        return dict;
    }
    else {
        return std::make_unique<Dictionary>(*m_dict);
    }
}

Dictionary::Ptr CrudAttributes::get_copy_as_dict(std::size_t index) const
{
    if (is_json()) {
        auto dict = Dictionary::create();
        m_json->write(*dict, Index(index));
        return dict;
    }
    else {
        return m_dict->get_copy_of_item(index);
    }
}

JsonDocument* CrudAttributes::get_json() const
{
    return m_json.get();
}

Dictionary* CrudAttributes::get_dict() const
{
    return m_dict.get();
}

CrudAttributes::Format CrudAttributes::get_format() const
{
    return m_format.m_type;
}

std::string CrudAttributes::get_format_as_string() const
{
    return FormatSpec::to_string(m_format.m_type);
}

void CrudAttributes::get_values(
    const SearchExpression& search, std::vector<std::string>& values) const
{
    if (is_json()) {
        m_json->get_values(search, values);
    }
    else {
        m_dict->get_values(search, values);
    }
}

bool CrudAttributes::is_json() const
{
    return m_format.is_json();
}

bool CrudAttributes::has_content() const
{
    return !m_json->is_empty() || !m_dict->is_empty();
}

const std::string& CrudAttributes::get_key_prefix() const
{
    return m_key_prefix;
}

std::vector<std::string> CrudAttributes::get_ids() const
{
    std::vector<std::string> ids;
    if (is_json()) {
        m_json->get_values(SearchExpression("id"), ids);
        if (ids.empty()) {
            ids.resize(m_json->get_size());
        }
    }
    else {
        m_dict->get_values(SearchExpression("id"), ids);
        if (ids.empty()) {
            ids.resize(m_dict->get_size());
        }
    }
    return ids;
}

void CrudAttributes::write(
    std::string& output, const FormatSpec& format, const Index& index) const
{
    if (m_format.is_none_type()) {
        return;
    }

    if (is_json()) {
        if (format.is_unset() || format.is_json()) {
            m_json->write(output, index, format.m_json_format);
        }
        else {
            Dictionary dict;
            m_json->write(dict, index, format.m_json_format);
            dict.write(output, {}, format.m_dict_format);
        }
    }
    else {
        if (format.is_unset()
            || format.m_type == CrudAttributes::Format::DICT) {
            m_dict->write(output, index, format.m_dict_format);
        }
        else {
            JsonDocument(*m_dict).write(output, index, format.m_json_format);
        }
    }
}

std::string CrudAttributes::to_string(
    const FormatSpec& format, const Index& index) const
{
    std::string output;
    write(output, format, index);
    return output;
}

void CrudAttributes::set_body(Dictionary::Ptr dict)
{
    if (m_format.is_unset()
        || m_format.m_type == CrudAttributes::Format::DICT) {
        m_dict = std::move(dict);
        m_format.set_is_dict();
    }
    else {
        m_json->load(*dict);
    }
}

void CrudAttributes::set_body(JsonDocument::Ptr json)
{
    if (m_format.is_unset() || is_json()) {
        m_json = std::move(json);
        m_format.set_is_json();
    }
    else {
        json->write(*m_dict);
    }
}

void CrudAttributes::set_format(const FormatSpec& format)
{
    m_format = format;
}

void CrudAttributes::set_key_prefix(const std::string& prefix)
{
    m_key_prefix = prefix;
}

}  // namespace hestia