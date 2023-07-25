#include "BaseCrudRequest.h"

namespace hestia {

const std::vector<CrudMethod> BaseCrudRequest::s_all_methods = {
    CrudMethod::CREATE, CrudMethod::READ,     CrudMethod::UPDATE,
    CrudMethod::REMOVE, CrudMethod::IDENTIFY, CrudMethod::LOCK,
    CrudMethod::UNLOCK, CrudMethod::LOCKED};

BaseCrudRequest::BaseCrudRequest(
    const VecCrudIdentifier& ids,
    const CrudAttributes& attributes,
    CrudQuery::OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    BaseRequest(), m_ids(ids)
{
    m_query.attributes() = attributes;
    m_query.set_output_format(output_format);
    m_query.set_attributes_output_format(attributes_format);
}

BaseCrudRequest::BaseCrudRequest(const CrudQuery& query) :
    BaseRequest(), m_query(query)
{
}

BaseCrudRequest::BaseCrudRequest(CrudLockType lock_type) :
    BaseRequest(), m_lock_type(lock_type)
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

const CrudQuery& BaseCrudRequest::get_query() const
{
    return m_query;
}

CrudLockType BaseCrudRequest::lock_type() const
{
    return m_lock_type;
}
}  // namespace hestia