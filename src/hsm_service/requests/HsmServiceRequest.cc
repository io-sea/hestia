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
    mExtent = extent;
}

void HsmServiceRequest::set_target_tier(uint8_t tier)
{
    mTargetTier = tier;
}

void HsmServiceRequest::set_source_tier(uint8_t tier)
{
    mSourceTier = tier;
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
    mQuery = query;
}

const ostk::StorageObject& HsmServiceRequest::object() const
{
    return mObject;
}