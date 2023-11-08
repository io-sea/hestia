#include "CrudQuery.h"

#include "StringUtils.h"

#include <stdexcept>

namespace hestia {

CrudQuery::CrudQuery(BodyFormat output_format) : m_output_format(output_format)
{
}

CrudQuery::CrudQuery(const CrudAttributes& attrs, BodyFormat output_format) :
    m_attributes(attrs), m_output_format(output_format)
{
    m_ids.add_primary_keys(m_attributes.get_ids());
}

CrudQuery::CrudQuery(
    const std::string& attrs_body,
    const CrudAttributes::FormatSpec& attrs_format,
    BodyFormat output_format) :
    m_attributes(attrs_body, attrs_format), m_output_format(output_format)
{
    m_ids.add_primary_keys(m_attributes.get_ids());
}

CrudQuery::CrudQuery(
    const CrudIdentifier& identifier,
    const CrudAttributes& attrs,
    BodyFormat output_format) :
    m_attributes(attrs), m_output_format(output_format)
{
    m_ids.add(identifier);
}

CrudQuery::CrudQuery(
    const CrudIdentifier& identifier, BodyFormat output_format) :
    m_output_format(output_format), m_format(Format::ID)
{
    m_ids.add(identifier);
}

CrudQuery::CrudQuery(
    const CrudIdentifierCollection& identifiers, BodyFormat output_format) :
    m_output_format(output_format), m_format(Format::ID), m_ids(identifiers)
{
}

CrudQuery::CrudQuery(
    const Map& filter, Format format, BodyFormat output_format) :
    m_output_format(output_format), m_format(format), m_filter(filter)
{
}

const Map& CrudQuery::get_filter() const
{
    return m_filter;
}

CrudQuery::Format CrudQuery::format_from_string(const std::string& input)
{
    return Format_enum_string_converter().init().from_string(input);
}

CrudAttributes& CrudQuery::attributes()
{
    return m_attributes;
}

bool CrudQuery::expects_single_item() const
{
    return (is_id() && m_ids.size() == 1)
           || (is_filter() && get_format() == CrudQuery::Format::GET);
}

CrudIdentifierCollection& CrudQuery::ids()
{
    return m_ids;
}

CrudQuery::BodyFormat CrudQuery::get_output_format() const
{
    return m_output_format;
}

CrudQuery::Format CrudQuery::get_format() const
{
    return m_format;
}

std::string CrudQuery::get_format_as_string() const
{
    return Format_enum_string_converter().init().to_string(m_format);
}

std::size_t CrudQuery::get_max_items() const
{
    return m_max_items;
}

std::size_t CrudQuery::get_offset() const
{
    return m_offset;
}

bool CrudQuery::is_filter() const
{
    return m_format == Format::GET || m_format == Format::LIST;
}

bool CrudQuery::is_id() const
{
    return m_format == Format::ID;
}

bool CrudQuery::is_json_output_format() const
{
    return m_output_format == BodyFormat::JSON;
}

bool CrudQuery::is_dict_output_format() const
{
    return m_output_format == BodyFormat::DICT;
}

bool CrudQuery::is_id_output_format() const
{
    return m_output_format == BodyFormat::ID;
}

bool CrudQuery::is_item_output_format() const
{
    return m_output_format == BodyFormat::ITEM;
}

bool CrudQuery::is_attribute_output_format() const
{
    return is_json_output_format() || is_dict_output_format();
}

void CrudQuery::set_output_format(BodyFormat output_format)
{
    m_output_format = output_format;
}

void CrudQuery::set_ids(const CrudIdentifierCollection& ids)
{
    m_ids    = ids;
    m_format = Format::ID;
}

void CrudQuery::set_ids(
    const std::string& buffer,
    const CrudIdentifierCollection::FormatSpec& format)
{
    m_ids.load(buffer, format);
}

void CrudQuery::set_filter(const Map& filter)
{
    m_filter = filter;
}

void CrudQuery::set_offset(std::size_t offset)
{
    m_offset = offset;
}

void CrudQuery::set_count(std::size_t count)
{
    m_count = count;
}

const CrudAttributes& CrudQuery::get_attributes() const
{
    return m_attributes;
}

void CrudQuery::append(
    const std::string& body, const CrudAttributes::FormatSpec& format)
{
    m_attributes.append(body, format);

    if (m_ids.empty()) {
        m_ids = CrudIdentifierCollection(m_attributes.get_ids());
    }
}


}  // namespace hestia