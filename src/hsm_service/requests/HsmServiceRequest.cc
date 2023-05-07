#include "HsmServiceRequest.h"

namespace hestia {
HsmServiceRequest::HsmServiceRequest(
    const hestia::Uuid& object_id, HsmServiceRequestMethod method) :
    hestia::MethodRequest<HsmServiceRequestMethod>(method)
{
    (void)object_id;
}

HsmServiceRequest::HsmServiceRequest(
    const hestia::StorageObject& object, HsmServiceRequestMethod method) :
    hestia::MethodRequest<HsmServiceRequestMethod>(method)
{
    (void)object;
}

HsmServiceRequest::HsmServiceRequest(HsmServiceRequestMethod method) :
    hestia::MethodRequest<HsmServiceRequestMethod>(method)
{
}

void HsmServiceRequest::set_extent(const hestia::Extent& extent)
{
    m_extent = extent;
}

void HsmServiceRequest::set_target_tier(uint8_t tier)
{
    m_target_tier = tier;
}

void HsmServiceRequest::set_source_tier(uint8_t tier)
{
    m_source_tier = tier;
}

void HsmServiceRequest::set_should_put_overwrite(bool overwrite)
{
    m_overwrite_if_existing = overwrite;
}

bool HsmServiceRequest::should_overwrite_put() const
{
    return m_overwrite_if_existing;
}

void HsmServiceRequest::set_query(const std::string& query)
{
    m_query = query;
}

const hestia::StorageObject& HsmServiceRequest::object() const
{
    return m_object;
}
}  // namespace hestia