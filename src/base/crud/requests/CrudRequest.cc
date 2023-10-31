#include "CrudRequest.h"

namespace hestia {

CrudRequest::CrudRequest(CrudMethod method) : MethodRequest<CrudMethod>(method)
{
}

CrudRequest::CrudRequest(
    CrudMethod method,
    VecModelPtr items,
    const CrudQuery& query,
    const CrudUserContext& user_context) :
    BaseCrudRequest(user_context, query),
    MethodRequest<CrudMethod>(method),
    m_items(std::move(items))
{
}

CrudRequest::CrudRequest(
    CrudMethod method,
    const CrudQuery& query,
    const CrudUserContext& user_context,
    bool update_event_feed) :
    BaseCrudRequest(user_context, query),
    MethodRequest<CrudMethod>(method),
    m_update_event_feed(update_event_feed)
{
}

CrudRequest::CrudRequest(
    CrudMethod method, const CrudUserContext& user_context) :
    BaseCrudRequest(user_context, {}), MethodRequest<CrudMethod>(method)
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

bool CrudRequest::has_attributes() const
{
    return m_query.get_attributes().has_content();
}

Model* CrudRequest::get_item() const
{
    return m_items[0].get();
}

bool CrudRequest::has_ids() const
{
    return !m_query.get_ids().empty();
}

Dictionary::Ptr CrudRequest::get_attribute_as_dict(std::size_t index) const
{
    return m_query.get_attributes().get_copy_as_dict(index);
}

const VecModelPtr& CrudRequest::items() const
{
    return m_items;
}

bool CrudRequest::is_create_method() const
{
    return m_method == CrudMethod::CREATE;
}

bool CrudRequest::is_read_method() const
{
    return m_method == CrudMethod::READ;
}

bool CrudRequest::is_identify_method() const
{
    return m_method == CrudMethod::IDENTIFY;
}

bool CrudRequest::is_update_method() const
{
    return m_method == CrudMethod::UPDATE;
}

bool CrudRequest::is_remove_method() const
{
    return m_method == CrudMethod::REMOVE;
}

bool CrudRequest::is_create_or_update_method() const
{
    return is_create_method() || is_update_method();
}

std::size_t CrudRequest::size() const
{
    return has_items() ? m_items.size() : get_ids().size();
}

void CrudRequest::write_body(
    const CrudAttributes::FormatSpec& format,
    std::string& buffer,
    const Index& index) const
{
    m_query.get_attributes().write(buffer, format, index);
}

}  // namespace hestia