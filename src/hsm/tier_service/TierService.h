#pragma once

#include "KeyValueCrudService.h"
#include "StorageTier.h"

#include <vector>

namespace hestia {
class KeyValueStoreClient;

struct TierServiceConfig {
    std::string m_endpoint;
};

class TierService : public KeyValueCrudService<StorageTier> {
  public:
    TierService(
        const TierServiceConfig& config, KeyValueStoreClient* kv_store_client);

    virtual ~TierService() = default;

    void to_string(const StorageTier& item, std::string& output) const override;

    void from_string(
        const std::string& output, StorageTier& item) const override;

  private:
    TierServiceConfig m_config;
};
}  // namespace hestia