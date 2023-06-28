#include "NamespaceService.h"

#include "HttpClient.h"
#include "HttpCrudClient.h"
#include "KeyValueCrudClient.h"
#include "KeyValueStoreClient.h"

#include "StringAdapter.h"

namespace hestia {

NamespaceService::NamespaceService(
    const NamespaceServiceConfig& config,
    std::unique_ptr<CrudClient<Namespace>> client) :
    CrudService<Namespace>(std::move(client)), m_config(config)
{
}

NamespaceService::Ptr NamespaceService::create(
    const NamespaceServiceConfig& config, KeyValueStoreClient* client)
{
    auto adapter = std::make_unique<JsonAdapter<Namespace>>();

    KeyValueCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<KeyValueCrudClient<Namespace>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<NamespaceService>(config, std::move(crud_client));
}

NamespaceService::Ptr NamespaceService::create(
    const NamespaceServiceConfig& config, HttpClient* client)
{
    auto adapter = std::make_unique<JsonAdapter<Namespace>>();

    HttpCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<HttpCrudClient<Namespace>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<NamespaceService>(config, std::move(crud_client));
}
}  // namespace hestia