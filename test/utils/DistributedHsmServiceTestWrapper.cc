#include "DistributedHsmServiceTestWrapper.h"

#include <catch2/catch_all.hpp>

#include "HsmService.h"
#include "StorageTier.h"
#include "TypedCrudRequest.h"

#include "TestUtils.h"
#include <iostream>

void DistributedHsmServiceTestWrapper::init(
    const std::string& user_name,
    const std::string& user_pass,
    std::size_t num_tiers)
{
    hestia::InMemoryObjectStoreClientConfig hsm_memory_client_config;

    std::vector<std::string> tier_names;
    for (std::size_t idx = 0; idx < num_tiers; idx++) {
        tier_names.push_back(std::to_string(idx));
    }
    m_obj_store_client.set_tier_names(tier_names);

    hestia::Dictionary serialized_config;
    hsm_memory_client_config.serialize(serialized_config);
    m_obj_store_client.initialize("0", {}, serialized_config);

    hestia::ObjectStoreBackend object_store_backend(
        hestia::ObjectStoreBackend::Type::MEMORY_HSM);
    object_store_backend.set_tier_names(tier_names);
    object_store_backend.set_config(serialized_config);

    hestia::KeyValueStoreCrudServiceBackend crud_backend(&m_kv_store_client);

    auto token_generator = std::make_unique<MockS3TokenGenerator>();
    m_token_generator    = token_generator.get();

    m_user_service = hestia::UserService::create(
        {}, &crud_backend, nullptr, nullptr, std::move(token_generator));

    auto register_response =
        m_user_service->register_user(user_name, user_pass);

    REQUIRE(register_response->ok());

    auto auth_response =
        m_user_service->authenticate_user(user_name, user_pass);
    REQUIRE(auth_response->ok());

    auto hsm_service = hestia::HsmService::create(
        hestia::ServiceConfig{}, &m_kv_store_client, &m_obj_store_client,
        m_user_service.get(), nullptr);

    auto tier_service = hsm_service->get_service(hestia::HsmItem::Type::TIER);

    for (std::size_t idx = 0; idx < 5; idx++) {
        hestia::StorageTier tier(idx);
        auto response = tier_service->make_request(hestia::TypedCrudRequest{
            hestia::CrudMethod::CREATE,
            tier,
            {},
            {m_user_service->get_current_user().get_primary_key()}});
        REQUIRE(response->ok());
    }
    hsm_service->update_tiers(
        m_user_service->get_current_user().get_primary_key());

    hestia::DistributedHsmServiceConfig dist_hsm_config;
    dist_hsm_config.m_is_server = true;
    dist_hsm_config.m_backends.push_back(object_store_backend);

    m_dist_hsm_service = hestia::DistributedHsmService::create(
        dist_hsm_config, std::move(hsm_service), m_user_service.get());
}