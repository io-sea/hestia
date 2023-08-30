#include "CrudQuery.h"
#include <stdexcept>

namespace hestia {

CrudQuery::CrudQuery(
    OutputFormat output_format, CrudAttributes::Format attributes_format) :
    m_output_format(output_format)
{
    m_attributes.set_format(attributes_format);
}

CrudQuery::CrudQuery(
    const CrudIdentifier& identifier,
    OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    m_output_format(output_format)
{
    m_attributes.set_format(attributes_format);
    m_ids    = {identifier};
    m_format = Format::ID;
}

CrudQuery::CrudQuery(
    const VecCrudIdentifier& identifiers,
    OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    m_output_format(output_format), m_ids(identifiers)
{
    m_attributes.set_format(attributes_format);
    m_format = Format::ID;
}

CrudQuery::CrudQuery(
    const Map& filter,
    Format format,
    OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    m_output_format(output_format), m_filter(filter)
{
    m_attributes.set_format(attributes_format);
    m_format = format;
}

CrudQuery::CrudQuery(
    const KeyValuePair& filter,
    Format format,
    OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    m_output_format(output_format)
{
    m_attributes.set_format(attributes_format);
    m_filter.set_item(filter.first, filter.second);
    m_format = format;
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
    return (is_id() && ids().size() == 1)
           || (is_filter() && get_format() == CrudQuery::Format::GET);
}

const VecCrudIdentifier& CrudQuery::ids() const
{
    return m_ids;
}

const CrudIdentifier& CrudQuery::get_id() const
{
    if (!has_single_id()) {
        throw std::runtime_error("Single crud id requested but none set");
    }
    return m_ids[0];
}

CrudQuery::OutputFormat CrudQuery::get_output_format() const
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

bool CrudQuery::has_single_id() const
{
    return m_ids.size() == 1;
}

bool CrudQuery::is_id_output_format() const
{
    return m_output_format == OutputFormat::ID;
}

bool CrudQuery::is_item_output_format() const
{
    return m_output_format == OutputFormat::ITEM;
}

bool CrudQuery::is_attribute_output_format() const
{
    return m_output_format == OutputFormat::ATTRIBUTES;
}

bool CrudQuery::is_dict_output_format() const
{
    return m_output_format == OutputFormat::DICT;
}

void CrudQuery::set_output_format(OutputFormat output_format)
{
    m_output_format = output_format;
}

void CrudQuery::set_attributes_output_format(CrudAttributes::Format format)
{
    m_attributes.set_format(format);
}

void CrudQuery::set_ids(const VecCrudIdentifier& ids)
{
    m_ids    = ids;
    m_format = Format::ID;
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


}  // namespace hestia