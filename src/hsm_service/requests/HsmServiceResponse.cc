#include "HsmServiceResponse.h"

HsmServiceResponse::HsmServiceResponse(const HsmServiceRequest& request) :
    ostk::Response<HsmServiceErrorCode>(request)
{
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request,
    HsmObjectStoreResponse::Ptr object_store_response) :
    ostk::Response<HsmServiceErrorCode>(request),
    m_object_store_response(std::move(object_store_response))
{
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request,
    ostk::ObjectStoreResponse::Ptr kv_store_response) :
    ostk::Response<HsmServiceErrorCode>(request),
    m_key_value_store_response(std::move(kv_store_response))
{
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request)
{
    return std::make_unique<HsmServiceResponse>(request);
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request,
    HsmObjectStoreResponse::Ptr object_store_response)
{
    return std::make_unique<HsmServiceResponse>(
        request, std::move(object_store_response));
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request,
    ostk::ObjectStoreResponse::Ptr kv_store_response)
{
    return std::make_unique<HsmServiceResponse>(
        request, std::move(kv_store_response));
}

const std::string& HsmServiceResponse::query_result() const
{
    return m_query_result;
}

const std::vector<ostk::Uuid>& HsmServiceResponse::objects() const
{
    return m_object_ids;
}

const std::vector<uint8_t>& HsmServiceResponse::tiers() const
{
    return m_tiers;
}