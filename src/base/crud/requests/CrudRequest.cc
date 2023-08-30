#include "CrudRequest.h"

namespace hestia {
CrudRequest::CrudRequest(
    CrudMethod method,
    VecModelPtr items,
    const CrudUserContext& user_id,
    CrudQuery::OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    BaseCrudRequest(user_id),
    MethodRequest<CrudMethod>(method),
    m_items(std::move(items))
{
    m_query.set_output_format(output_format);
    m_query.set_attributes_output_format(attributes_format);
}

CrudRequest::CrudRequest(
    CrudMethod method,
    const CrudUserContext& user_id,
    const VecCrudIdentifier& ids,
    const CrudAttributes& attributes,
    CrudQuery::OutputFormat output_format,
    CrudAttributes::Format attributes_format) :
    BaseCrudRequest(user_id, ids, attributes, output_format, attributes_format),
    MethodRequest<CrudMethod>(method)
{
}

CrudRequest::CrudRequest(
    const CrudQuery& query,
    const CrudUserContext& user_id,
    bool update_event_feed) :
    BaseCrudRequest(user_id, query),
    MethodRequest<CrudMethod>(CrudMethod::READ),
    m_update_event_feed(update_event_feed)
{
}

CrudRequest::CrudRequest(
    CrudMethod method, CrudLockType lock_type, const CrudUserContext& user_id) :
    BaseCrudRequest(user_id, lock_type), MethodRequest<CrudMethod>(method)
{
}

CrudRequest::~CrudRequest() {}

std::string CrudRequest::method_as_string() const
{
    switch (m_method) {
        case CrudMethod::CREATE:
            return "CREATE";
        case CrudMethod::UPDATE:
            return "UPDATE";
        case CrudMethod::READ:
            return "READ";
        case CrudMethod::REMOVE:
            return "REMOVE";
        case CrudMethod::IDENTIFY:
            return "IDENTIFY";
        case CrudMethod::LOCK:
            return "LOCK";
        case CrudMethod::UNLOCK:
            return "UNLOCK";
        case CrudMethod::LOCKED:
            return "LOCKED";
        default:
            return "UNKNOWN";
    }
}

bool CrudRequest::has_items() const
{
    return !m_items.empty();
}

Model* CrudRequest::get_item() const
{
    return m_items[0].get();
}

const VecModelPtr& CrudRequest::items() const
{
    return m_items;
}
}  // namespace hestia