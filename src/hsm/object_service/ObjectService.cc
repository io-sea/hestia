#include "ObjectService.h"

#include "HttpClient.h"
#include "HttpCrudClient.h"
#include "KeyValueCrudClient.h"
#include "KeyValueStoreClient.h"

#include "StringAdapter.h"

namespace hestia {

ObjectService::ObjectService(
    const ObjectServiceConfig& config,
    std::unique_ptr<CrudClient<HsmObject>> client) :
    CrudService<HsmObject>(std::move(client)), m_config(config)
{
}

ObjectService::~ObjectService() {}

ObjectService::Ptr ObjectService::create(
    const ObjectServiceConfig& config, KeyValueStoreClient* client)
{
    auto adapter = std::make_unique<JsonAdapter<HsmObject>>();

    KeyValueCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<KeyValueCrudClient<HsmObject>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<ObjectService>(config, std::move(crud_client));
}

ObjectService::Ptr ObjectService::create(
    const ObjectServiceConfig& config, HttpClient* client)
{
    auto adapter = std::make_unique<JsonAdapter<HsmObject>>();

    HttpCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<HttpCrudClient<HsmObject>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<ObjectService>(config, std::move(crud_client));
}

}  // namespace hestia