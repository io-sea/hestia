#include "HsmServiceRequest.h"

HsmServiceRequest::HsmServiceRequest(
    const ostk::Uuid& objectId, HsmServiceRequestMethod method) :
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

void HsmServiceRequest::setExtent(const ostk::Extent& extent)
{
    mExtent = extent;
}

void HsmServiceRequest::setTargetTier(uint8_t tier)
{
    mTargetTier = tier;
}

void HsmServiceRequest::setSourceTier(uint8_t tier)
{
    mSourceTier = tier;
}

void HsmServiceRequest::setShouldPutOverwrite(bool overwrite)
{
    mOverwriteIfExisting = overwrite;
}

bool HsmServiceRequest::shouldOverwritePut() const
{
    return mOverwriteIfExisting;
}

void HsmServiceRequest::setQuery(const std::string& query)
{
    mQuery = query;
}

const ostk::StorageObject& HsmServiceRequest::object() const
{
    return mObject;
}