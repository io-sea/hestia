#include "TierService.h"

#include "HttpClient.h"
#include "HttpCrudClient.h"
#include "KeyValueCrudClient.h"
#include "KeyValueStoreClient.h"

#include "StringAdapter.h"

namespace hestia {

TierService::TierService(
    const TierServiceConfig& config,
    std::unique_ptr<CrudClient<StorageTier>> client) :
    CrudService<StorageTier>(std::move(client)), m_config(config)
{
}

TierService::Ptr TierService::create(
    const TierServiceConfig& config, KeyValueStoreClient* client)
{
    auto adapter = std::make_unique<JsonAdapter<StorageTier>>();

    KeyValueCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<KeyValueCrudClient<StorageTier>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<TierService>(config, std::move(crud_client));
}

TierService::Ptr TierService::create(
    const TierServiceConfig& config, HttpClient* client)
{
    auto adapter = std::make_unique<JsonAdapter<StorageTier>>();

    HttpCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<HttpCrudClient<StorageTier>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<TierService>(config, std::move(crud_client));
}
}  // namespace hestia