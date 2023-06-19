#include "HsmServiceResponse.h"

namespace hestia {
HsmServiceResponse::HsmServiceResponse(const HsmServiceRequest& request) :
    hestia::Response<HsmServiceErrorCode>(request),
    m_object_response(std::make_unique<ObjectServiceResponse>(request)),
    m_tier_response(std::make_unique<TierServiceResponse>(request))
{
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request,
    HsmObjectStoreResponse::Ptr object_store_response) :
    hestia::Response<HsmServiceErrorCode>(request),
    m_object_store_response(std::move(object_store_response))
{
    if (!m_object_store_response->ok()) {
        on_error(
            {HsmServiceErrorCode::ERROR,
             m_object_store_response->get_base_error().to_string()});
    }
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request,
    ObjectServiceResponsePtr object_service_response) :
    hestia::Response<HsmServiceErrorCode>(request),
    m_object_response(std::move(object_service_response)),
    m_tier_response(std::make_unique<TierServiceResponse>(request))
{
    if (!m_object_response->ok()) {
        on_error(
            {HsmServiceErrorCode::ERROR,
             m_object_response->get_base_error().to_string()});
    }
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request,
    TierServiceResponsePtr tier_service_response) :
    hestia::Response<HsmServiceErrorCode>(request),
    m_object_response(std::make_unique<ObjectServiceResponse>(request)),
    m_tier_response(std::move(tier_service_response))
{
    if (!m_tier_response->ok()) {
        on_error(
            {HsmServiceErrorCode::ERROR,
             m_tier_response->get_base_error().to_string()});
    }
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
    ObjectServiceResponsePtr object_service_response)
{
    return std::make_unique<HsmServiceResponse>(
        request, std::move(object_service_response));
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request,
    TierServiceResponsePtr tier_service_response)
{
    return std::make_unique<HsmServiceResponse>(
        request, std::move(tier_service_response));
}

const std::string& HsmServiceResponse::query_result() const
{
    return m_query_result;
}

const std::vector<HsmObject>& HsmServiceResponse::objects() const
{
    if(m_object_response==nullptr){
      return m_objects;
    }
    return m_object_response->items();
}

const std::vector<StorageTier>& HsmServiceResponse::tiers() const
{
    if(m_tier_response==nullptr){
      return m_tiers;
    }
    return m_tier_response->items();
}

const HsmObject& HsmServiceResponse::object() const
{
    return m_object_response->item();
}

HsmObject& HsmServiceResponse::object()
{
    return m_object_response->item();
}

bool HsmServiceResponse::object_found() const
{
    return m_object_response->found();
}

void HsmServiceResponse::set_object_found(bool found)
{
    m_object_response->set_found(found);
}

void HsmServiceResponse::add_tier(const StorageTier& tier)
{
   m_tiers.push_back(tier);
}

void HsmServiceResponse::add_object(const HsmObject& object)
{
   m_object_response->items().push_back(object);
}

}  // namespace hestia
