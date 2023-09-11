#include "BaseCrudRequest.h"

namespace hestia {

BaseCrudRequest::BaseCrudRequest(
    const CrudUserContext& user_context,
    const VecCrudIdentifier& ids,
    const CrudAttributes& attributes,
    CrudQuery::OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    BaseRequest(), m_user_context(user_context), m_ids(ids)
{
    m_query.attributes() = attributes;
    m_query.set_output_format(output_format);

    m_query.set_attributes_output_format(attributes_format);
}

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

const CrudAttributes& BaseCrudRequest::get_attributes() const
{
    return m_query.get_attributes();
}

const VecCrudIdentifier& BaseCrudRequest::get_ids() const
{
    return m_ids;
}

const CrudUserContext& BaseCrudRequest::get_user_context() const
{
    return m_user_context;
}

const CrudQuery& BaseCrudRequest::get_query() const
{
    return m_query;
}

CrudLockType BaseCrudRequest::lock_type() const
{
    return m_lock_type;
}
}  // namespace hestia