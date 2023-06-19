#include "DistributedHsmServiceTestWrapper.h"

#include <catch2/catch_all.hpp>

#include "HsmService.h"
#include "TierService.h"

#include "TestUtils.h"

DistributedHsmServiceTestWrapper::DistributedHsmServiceTestWrapper(
    const std::string& test_file, const std::string& test_name)
{
    m_store_path = TestUtils::get_test_output_dir(test_file) / test_name;
    std::filesystem::remove_all(m_store_path);

    hestia::Metadata store_config;
    store_config.set_item("root", m_store_path);

    m_kv_store_client.initialize(store_config);
    m_obj_store_client.initialize(store_config);

    auto hsm_service =
        hestia::HsmService::create(&m_kv_store_client, &m_obj_store_client);

    hestia::HsmObjectStoreClientBackend object_store_backend(
        hestia::HsmObjectStoreClientBackend::Type::HSM,
        hestia::HsmObjectStoreClientBackend::Source::BUILT_IN,
        "hestia::FileHsmObjectStoreClient");

    hestia::DistributedHsmServiceConfig dist_hsm_config;
    dist_hsm_config.m_self.m_backends = {object_store_backend};
    m_dist_hsm_service                = hestia::DistributedHsmService::create(
        dist_hsm_config, std::move(hsm_service), &m_kv_store_client);
}

DistributedHsmServiceTestWrapper::~DistributedHsmServiceTestWrapper()
{
    // std::filesystem::remove_all(m_store_path);
}

void DistributedHsmServiceTestWrapper::add_tiers(std::size_t num_tiers)
{
    auto tier_service =
        m_dist_hsm_service->get_hsm_service()->get_tier_service();
    for (std::size_t idx = 0; idx < num_tiers; idx++) {
        auto response = tier_service->make_request(
            {hestia::StorageTier(idx), hestia::CrudMethod::PUT});
        REQUIRE(response->ok());
    }
}