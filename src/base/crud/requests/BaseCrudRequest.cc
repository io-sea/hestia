#include "BaseCrudRequest.h"

namespace hestia {

BaseCrudRequest::BaseCrudRequest(
    const CrudUserContext& user_context, const CrudQuery& query) :
    BaseRequest(), m_user_context(user_context), m_query(query)
{
}

BaseCrudRequest::BaseCrudRequest(
    const CrudUserContext& user_context, CrudLockType lock_type) :
    BaseRequest(), m_user_context(user_context), m_lock_type(lock_type)
{
}

std::vector<std::string> BaseCrudRequest::get_crud_methods()
{
    std::vector<std::string> methods;
    CrudMethod_enum_string_converter converter;
    converter.init();
    for (const auto& method : BaseCrudRequest::s_all_methods) {
        methods.push_back(converter.to_string(method));
    }
    return methods;
}

const CrudAttributes& BaseCrudRequest::get_attributes() const
{
    return m_query.get_attributes();
}

const CrudIdentifierCollection& BaseCrudRequest::get_ids() const
{
    return m_query.get_ids();
}

const CrudUserContext& BaseCrudRequest::get_user_context() const
{
    return m_user_context;
}

const CrudQuery& BaseCrudRequest::get_query() const
{
    return m_query;
}

const CrudQuery::OutputFormat& BaseCrudRequest::get_output_format() const
{
    return m_query.get_output_format();
}

void BaseCrudRequest::set_ids(const CrudIdentifierCollection& ids)
{
    m_query.set_ids(ids);
}

void BaseCrudRequest::set_query_filter(const Map& filter)
{
    m_query.set_filter(filter);
}

void BaseCrudRequest::set_ids(
    const std::string& buffer,
    const CrudIdentifierCollection::FormatSpec& format)
{
    m_query.set_ids(buffer, format);
}

void BaseCrudRequest::append(
    const std::string& body, const CrudAttributes::FormatSpec& format)
{
    m_query.append(body, format);
}

void BaseCrudRequest::set_output_format(const CrudQuery::OutputFormat& format)
{
    m_query.set_output_format(format);
}

void BaseCrudRequest::set_offset_and_count(
    std::size_t offset, std::size_t count)
{
    m_query.set_count(count);
    m_query.set_offset(offset);
}

CrudLockType BaseCrudRequest::lock_type() const
{
    return m_lock_type;
}
}  // namespace hestia