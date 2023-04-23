#pragma once

#include "HsmObjectStoreClientRegistry.h"

#include <unordered_map>

class HsmObjectStoreClientManager {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientManager>;

    HsmObjectStoreClientManager(
        HsmObjectStoreClientRegistry::Ptr client_registry);

    void setup_clients(const TierBackendRegistry& tier_backend_regsitry);

    ostk::ObjectStoreClient* get_client(
        ObjectStoreClientType client_type) const;

    ostk::ObjectStoreClient* get_client(uint8_t tier_id) const;

    HsmObjectStoreClient* get_hsm_client(
        ObjectStoreClientType client_type) const;

    HsmObjectStoreClient* get_hsm_client(uint8_t tier_id) const;

    bool is_hsm_client(uint8_t tier_id) const;

    bool have_same_client_types(uint8_t tier_id0, uint8_t tier_id1) const;

  private:
    HsmObjectStoreClientRegistry::Ptr m_client_registry;
    TierBackendRegistry m_tier_backend_registry;
    std::unordered_map<std::string, ostk::ObjectStoreClient::Ptr> m_clients;
    std::unordered_map<std::string, HsmObjectStoreClient::Ptr> m_hsm_clients;
};