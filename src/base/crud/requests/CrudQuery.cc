#include "CrudQuery.h"
#include <stdexcept>

namespace hestia {

CrudQuery::CrudQuery(
    OutputFormat output_format, CrudAttributes::Format attributes_format) :
    m_attributes_output_format(attributes_format),
    m_output_format(output_format)
{
}

CrudQuery::CrudQuery(
    const CrudIdentifier& identifier,
    OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    m_attributes_output_format(attributes_format),
    m_output_format(output_format)
{
    m_input_ids    = {identifier};
    m_input_format = InputFormat::Id;
}

CrudQuery::CrudQuery(
    const VecCrudIdentifier& identifiers,
    OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    m_attributes_output_format(attributes_format),
    m_output_format(output_format),
    m_input_ids(identifiers)
{
    m_input_format = InputFormat::Id;
}

CrudQuery::CrudQuery(
    const KeyValuePair& index,
    OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    m_attributes_output_format(attributes_format),
    m_output_format(output_format),
    m_input_index(index)
{
    m_input_format = InputFormat::Index;
}

const Map& CrudQuery::get_filter() const
{
    return m_input_filter;
}

const VecCrudIdentifier& CrudQuery::ids() const
{
    return m_input_ids;
}

const CrudIdentifier& CrudQuery::get_id() const
{
    if (!has_single_id()) {
        throw std::runtime_error("Single crud id requested but none set");
    }
    return m_input_ids[0];
}

const KeyValuePair& CrudQuery::get_index() const
{
    return m_input_index;
}

bool CrudQuery::is_filter() const
{
    return m_input_format == InputFormat::Filter;
}

bool CrudQuery::is_id() const
{
    return m_input_format == InputFormat::Id;
}

bool CrudQuery::is_index() const
{
    return m_input_format == InputFormat::Index;
}

bool CrudQuery::has_single_id() const
{
    return m_input_ids.size() == 1;
}
}  // namespace hestia