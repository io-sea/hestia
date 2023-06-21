#include "DatasetService.h"

#include "HttpClient.h"
#include "HttpCrudClient.h"
#include "KeyValueCrudClient.h"
#include "KeyValueStoreClient.h"

#include "DatasetAdapter.h"

namespace hestia {

DatasetService::DatasetService(
    const DatasetServiceConfig& config,
    std::unique_ptr<CrudClient<Dataset>> client) :
    CrudService<Dataset>(std::move(client)), m_config(config)
{
}

DatasetService::Ptr DatasetService::create(
    const DatasetServiceConfig& config, KeyValueStoreClient* client)
{
    auto adapter = std::make_unique<DatasetJsonAdapter>();

    KeyValueCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<KeyValueCrudClient<Dataset>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<DatasetService>(config, std::move(crud_client));
}

DatasetService::Ptr DatasetService::create(
    const DatasetServiceConfig& config, HttpClient* client)
{
    auto adapter = std::make_unique<DatasetJsonAdapter>();

    HttpCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<HttpCrudClient<Dataset>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<DatasetService>(config, std::move(crud_client));
}
}  // namespace hestia