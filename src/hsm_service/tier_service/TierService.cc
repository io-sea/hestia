#include "TierService.h"

#include "KeyValueStoreClient.h"
#include "Metadata.h"

#include "JsonUtils.h"

namespace hestia {

TierService::TierService(
    const TierServiceConfig& config, KeyValueStoreClient* kv_store_client) :
    KeyValueCrudService<StorageTier>(kv_store_client), m_config(config)
{
}

void TierService::to_string(const StorageTier& item, std::string& output) const
{
    (void)item;
    (void)output;
}

void TierService::from_string(
    const std::string& output, StorageTier& item) const
{
    (void)item;
    (void)output;
}
}  // namespace hestia