#pragma once

#include "StorageTier.h"
#include <vector>

namespace hestia {
class KeyValueStoreClient;

struct TierServiceConfig {
    std::string m_key_prefix{"tier_service"};
    std::string m_tier_key{"tier"};
};

class TierService {
  public:
    TierService(
        const TierServiceConfig& config, KeyValueStoreClient* kv_store_client);

    void add_tier(const StorageTier& tier) const;

    void add_tiers(const std::vector<StorageTier>& tiers) const;

    void get_tiers(std::vector<StorageTier>& tiers) const;

    void get_tier(uint8_t tier_id, StorageTier& tier) const;

  private:
    std::string get_key_prefix() const;

    TierServiceConfig m_config;
    KeyValueStoreClient* m_kv_store_client{nullptr};
};
}  // namespace hestia