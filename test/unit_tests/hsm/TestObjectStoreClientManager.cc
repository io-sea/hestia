#include <catch2/catch_all.hpp>

#include "ObjectStoreBackend.h"
#include "ObjectStoreClientFactory.h"
#include "ObjectStoreClientManager.h"
#include "StorageTier.h"

#include <string.h>

class ObjectStoreClientManagerTestFixture {
  public:
    ObjectStoreClientManagerTestFixture()
    {
        auto client_factory =
            std::make_unique<hestia::ObjectStoreClientFactory>(nullptr);
        m_client_manager = std::make_unique<hestia::ObjectStoreClientManager>(
            std::move(client_factory));
    }
    std::unique_ptr<hestia::ObjectStoreClientManager> m_client_manager;
};

TEST_CASE_METHOD(
    ObjectStoreClientManagerTestFixture,
    "Object Store Client Manager Test",
    "[object-store-client-manager]")
{
    /*
    std::vector<hestia::StorageTier> tiers;
    std::vector<std::string> tier_ids0, tier_ids1;
    for (std::size_t idx = 0; idx < 5; idx++) {
        hestia::StorageTier tier(std::to_string(idx));
        tier.set_priority(idx);
        if (idx < 4) {
            tier_ids0.push_back(tier.id());
        }
        else {
            tier_ids1.push_back(tier.id());
        }
        tiers.push_back(tier);
    }
    std::vector<hestia::ObjectStoreBackend> backends;
    hestia::ObjectStoreBackend backend0(
        hestia::ObjectStoreBackend::Type::FILE_HSM);
    backend0.set_tier_ids(tier_ids0);
    backends.push_back(backend0);

    hestia::ObjectStoreBackend backend1(hestia::ObjectStoreBackend::Type::FILE);
    backend1.set_tier_ids(tier_ids1);
    backends.push_back(backend1);

    m_client_manager->setup_clients("", "", nullptr, tiers, backends);

    hestia::HsmObjectStoreClient* hsm_client0;
    hestia::HsmObjectStoreClient* hsm_client1;
    hestia::HsmObjectStoreClient* hsm_client2;
    hsm_client0 = m_client_manager->get_hsm_client(tier_ids0[0]);
    hsm_client1 = m_client_manager->get_hsm_client(tier_ids0[1]);
    hsm_client2 = m_client_manager->get_hsm_client(tier_ids1[0]);

    hestia::ObjectStoreClient* client;
    client = m_client_manager->get_client(tier_ids1[0]);

    REQUIRE(hsm_client0);
    REQUIRE(hsm_client1);
    REQUIRE(hsm_client0 == hsm_client1);
    REQUIRE(!hsm_client2);
    REQUIRE(client);
    */
}
