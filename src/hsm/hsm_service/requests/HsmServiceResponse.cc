#include "HsmServiceResponse.h"

namespace hestia {
HsmServiceResponse::HsmServiceResponse(const HsmServiceRequest& request) :
    Response<HsmServiceErrorCode>(request),
    m_object_response(std::make_unique<ObjectServiceResponse>(request)),
    m_tier_response(std::make_unique<TierServiceResponse>(request)),
    m_dataset_response(std::make_unique<DatasetServiceResponse>(request))
{
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request, HsmObjectStoreResponse::Ptr response) :
    Response<HsmServiceErrorCode>(request),
    m_object_store_response(std::move(response))
{
    if (!m_object_store_response->ok()) {
        on_error(
            {HsmServiceErrorCode::ERROR,
             m_object_store_response->get_base_error().to_string()});
    }
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request, ObjectServiceResponsePtr response) :
    Response<HsmServiceErrorCode>(request),
    m_object_response(std::move(response)),
    m_tier_response(std::make_unique<TierServiceResponse>(request)),
    m_dataset_response(std::make_unique<DatasetServiceResponse>(request))
{
    if (!m_object_response->ok()) {
        on_error(
            {HsmServiceErrorCode::ERROR,
             m_object_response->get_base_error().to_string()});
    }
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request, TierServiceResponsePtr response) :
    Response<HsmServiceErrorCode>(request),
    m_object_response(std::make_unique<ObjectServiceResponse>(request)),
    m_tier_response(std::move(response)),
    m_dataset_response(std::make_unique<DatasetServiceResponse>(request))
{
    if (!m_tier_response->ok()) {
        on_error(
            {HsmServiceErrorCode::ERROR,
             m_tier_response->get_base_error().to_string()});
    }
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request, DatasetServiceResponsePtr response) :
    Response<HsmServiceErrorCode>(request),
    m_object_response(std::make_unique<ObjectServiceResponse>(request)),
    m_tier_response(std::make_unique<TierServiceResponse>(request)),
    m_dataset_response(std::move(response))
{
    if (!m_dataset_response->ok()) {
        on_error(
            {HsmServiceErrorCode::ERROR,
             m_dataset_response->get_base_error().to_string()});
    }
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request)
{
    return std::make_unique<HsmServiceResponse>(request);
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request, HsmObjectStoreResponse::Ptr response)
{
    return std::make_unique<HsmServiceResponse>(request, std::move(response));
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request, ObjectServiceResponsePtr response)
{
    return std::make_unique<HsmServiceResponse>(request, std::move(response));
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request, TierServiceResponsePtr response)
{
    return std::make_unique<HsmServiceResponse>(request, std::move(response));
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request, DatasetServiceResponsePtr response)
{
    return std::make_unique<HsmServiceResponse>(request, std::move(response));
}

const std::string& HsmServiceResponse::query_result() const
{
    return m_query_result;
}

const std::vector<HsmObject>& HsmServiceResponse::objects() const
{
    if (m_object_response == nullptr) {
        return m_objects;
    }
    return m_object_response->items();
}

const std::vector<StorageTier>& HsmServiceResponse::tiers() const
{
    if (m_tier_response == nullptr) {
        return m_tiers;
    }
    return m_tier_response->items();
}

const HsmObject& HsmServiceResponse::object() const
{
    return m_object_response->item();
}

const Dataset& HsmServiceResponse::dataset() const
{
    return m_dataset_response->item();
}

const std::vector<Dataset>& HsmServiceResponse::datasets() const
{
    return m_dataset_response->items();
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
