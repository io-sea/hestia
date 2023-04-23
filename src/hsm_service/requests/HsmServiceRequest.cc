#include "HsmServiceRequest.h"

HsmServiceRequest::HsmServiceRequest(
    const ostk::Uuid& object_id, HsmServiceRequestMethod method) :
    ostk::MethodRequest<HsmServiceRequestMethod>(method)
{
}

HsmServiceRequest::HsmServiceRequest(
    const ostk::StorageObject& object, HsmServiceRequestMethod method) :
    ostk::MethodRequest<HsmServiceRequestMethod>(method)
{
}

HsmServiceRequest::HsmServiceRequest(HsmServiceRequestMethod method) :
    ostk::MethodRequest<HsmServiceRequestMethod>(method)
{
}

void HsmServiceRequest::set_extent(const ostk::Extent& extent)
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

const ostk::StorageObject& HsmServiceRequest::object() const
{
    return m_object;
}