#include "DistributedHsmServiceTestWrapper.h"

#include <catch2/catch_all.hpp>

#include "HsmService.h"
#include "StorageTier.h"
#include "TypedCrudRequest.h"

#include "TestUtils.h"

DistributedHsmServiceTestWrapper::DistributedHsmServiceTestWrapper()
{
    hestia::KeyValueStoreCrudServiceBackend backend(&m_kv_store_client);
    m_user_service = hestia::UserService::create({}, &backend);

    auto hsm_service = hestia::HsmService::create(
        {}, &m_kv_store_client, &m_obj_store_client, m_user_service.get(),
        nullptr);

    hestia::ObjectStoreBackend object_store_backend(
        hestia::ObjectStoreBackend::Type::MEMORY);

    hestia::DistributedHsmServiceConfig dist_hsm_config;
    dist_hsm_config.m_backends.push_back(object_store_backend);
    m_dist_hsm_service = hestia::DistributedHsmService::create(
        dist_hsm_config, std::move(hsm_service), m_user_service.get());
}

void DistributedHsmServiceTestWrapper::add_tiers(std::size_t num_tiers)
{
    auto tier_service = m_dist_hsm_service->get_hsm_service()->get_service(
        hestia::HsmItem::Type::TIER);
    for (std::size_t idx = 0; idx < num_tiers; idx++) {
        auto response = tier_service->make_request(
            hestia::TypedCrudRequest<hestia::StorageTier>{
                hestia::CrudMethod::CREATE, hestia::StorageTier(idx), {}});
        REQUIRE(response->ok());
    }
}