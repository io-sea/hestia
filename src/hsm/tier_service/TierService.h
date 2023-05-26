#pragma once

#include "CrudService.h"
#include "StorageTier.h"

#include <memory>

namespace hestia {

class KeyValueStoreClient;
class HttpClient;

struct TierServiceConfig {
    std::string m_endpoint;
    std::string m_global_prefix;
    std::string m_item_prefix{"tier"};
};

class TierService : public CrudService<StorageTier> {
  public:
    using Ptr = std::unique_ptr<TierService>;

    TierService(
        const TierServiceConfig& config,
        std::unique_ptr<CrudClient<StorageTier>> client);

    static Ptr create(
        const TierServiceConfig& config, KeyValueStoreClient* client);

    static Ptr create(const TierServiceConfig& config, HttpClient* client);

    virtual ~TierService() = default;

  private:
    TierServiceConfig m_config;
};
}  // namespace hestia