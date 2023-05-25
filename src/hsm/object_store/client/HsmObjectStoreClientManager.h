#pragma once

#include "HsmObjectStoreClientFactory.h"

#include <unordered_map>

namespace hestia {
class HsmObjectStoreClientManager {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientManager>;

    HsmObjectStoreClientManager(
        HsmObjectStoreClientFactory::Ptr client_factory);

    void setup_clients(const TierBackendRegistry& tier_backend_regsitry);

    ObjectStoreClient* get_client(const HsmObjectStoreClientSpec& spec) const;

    ObjectStoreClient* get_client(uint8_t tier_id) const;

    HsmObjectStoreClient* get_hsm_client(
        const HsmObjectStoreClientSpec& spec) const;

    HsmObjectStoreClient* get_hsm_client(uint8_t tier_id) const;

    bool is_hsm_client(uint8_t tier_id) const;

    bool have_same_client_types(uint8_t tier_id0, uint8_t tier_id1) const;

  private:
    HsmObjectStoreClientFactory::Ptr m_client_factory;
    TierBackendRegistry m_tier_backend_registry;

    std::unordered_map<std::string, ObjectStoreClient::Ptr> m_clients;
    std::unordered_map<std::string, ObjectStoreClientPlugin::Ptr>
        m_plugin_clients;

    std::unordered_map<std::string, HsmObjectStoreClient::Ptr> m_hsm_clients;
    std::unordered_map<std::string, HsmObjectStoreClientPlugin::Ptr>
        m_hsm_plugin_clients;
};
}  // namespace hestia