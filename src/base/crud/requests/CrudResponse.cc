#include "CrudResponse.h"

#include <cassert>
#include <stdexcept>

namespace hestia {
CrudResponse::CrudResponse(const BaseRequest& request) :
    Response<CrudErrorCode>(request)
{
}

CrudResponse::~CrudResponse() {}

const CrudAttributes& CrudResponse::attributes() const
{
    return m_attributes;
}

CrudAttributes& CrudResponse::attributes()
{
    return m_attributes;
}

Model* CrudResponse::get_item() const
{
    if (m_items.empty()) {
        throw std::runtime_error(
            "Attempted to get item in CrudResponse but none set. Use found() to check first");
    }
    return m_items[0].get();
}

const VecModelPtr& CrudResponse::items() const
{
    return m_items;
}

VecModelPtr& CrudResponse::items()
{
    return m_items;
}

bool CrudResponse::found() const
{
    return !m_items.empty() || !m_ids.empty();
}

void CrudResponse::set_item(std::unique_ptr<Model>& item)
{
    assert(item != nullptr);
    m_items.push_back(std::move(item));
}

void CrudResponse::set_locked(bool is_locked)
{
    m_locked = is_locked;
}

bool CrudResponse::locked() const
{
    return m_locked;
}
}  // namespace hestia