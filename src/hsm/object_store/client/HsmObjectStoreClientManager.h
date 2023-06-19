#pragma once

#include "HsmObjectStoreClientFactory.h"
#include "StorageTier.h"

#include <unordered_map>

namespace hestia {
class HsmObjectStoreClientManager {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientManager>;

    HsmObjectStoreClientManager(
        HsmObjectStoreClientFactory::Ptr client_factory);

    ObjectStoreClient* get_client(const std::string& identifier) const;

    ObjectStoreClient* get_client(uint8_t tier_id) const;

    HsmObjectStoreClient* get_hsm_client(const std::string& identifier) const;

    HsmObjectStoreClient* get_hsm_client(uint8_t tier_id) const;

    std::string get_client_backend(uint8_t tier_id) const;

    bool has_client(uint8_t tier_id) const;

    bool have_same_client_types(uint8_t tier_id0, uint8_t tier_id1) const;

    bool is_hsm_client(uint8_t tier_id) const;

    void setup_clients(
        const std::unordered_map<std::string, HsmObjectStoreClientBackend>&
            backends,
        const std::unordered_map<uint8_t, StorageTier>& tiers);

  private:
    HsmObjectStoreClientFactory::Ptr m_client_factory;
    std::unordered_map<std::string, HsmObjectStoreClientBackend> m_backends;
    std::unordered_map<uint8_t, StorageTier> m_tiers;

    std::unordered_map<std::string, ObjectStoreClient::Ptr> m_clients;
    std::unordered_map<std::string, ObjectStoreClientPlugin::Ptr>
        m_plugin_clients;

    std::unordered_map<std::string, HsmObjectStoreClient::Ptr> m_hsm_clients;
    std::unordered_map<std::string, HsmObjectStoreClientPlugin::Ptr>
        m_hsm_plugin_clients;
};
}  // namespace hestia