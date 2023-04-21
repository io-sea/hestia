#include "HsmServiceResponse.h"

HsmServiceResponse::HsmServiceResponse(const HsmServiceRequest& request) :
    ostk::Response<HsmServiceErrorCode>(request)
{
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request,
    HsmObjectStoreResponse::Ptr object_store_response) :
    ostk::Response<HsmServiceErrorCode>(request),
    mObjectStoreResponse(std::move(objectStoreResponse))
{
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request,
    ostk::ObjectStoreResponse::Ptr kv_store_response) :
    ostk::Response<HsmServiceErrorCode>(request),
    mKeyValueStoreResponse(std::move(kvStoreResponse))
{
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request)
{
    return std::make_unique<HsmServiceResponse>(request);
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request,
    HsmObjectStoreResponse::Ptr objectStoreResponse)
{
    return std::make_unique<HsmServiceResponse>(
        request, std::move(objectStoreResponse));
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request,
    ostk::ObjectStoreResponse::Ptr kvStoreResponse)
{
    return std::make_unique<HsmServiceResponse>(
        request, std::move(kvStoreResponse));
}

const std::string& HsmServiceResponse::query_result() const
{
    return mQueryResult;
}

const std::vector<ostk::Uuid>& HsmServiceResponse::objects() const
{
    return mObjectIds;
}

const std::vector<uint8_t>& HsmServiceResponse::tiers() const
{
    return mTiers;
}