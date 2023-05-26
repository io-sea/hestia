#include "HsmNodeService.h"

#include "Logger.h"

#include "HttpClient.h"
#include "HttpCrudClient.h"
#include "KeyValueCrudClient.h"
#include "KeyValueStoreClient.h"

#include "HsmNodeAdapter.h"

namespace hestia {
HsmNodeService::HsmNodeService(
    const HsmNodeServiceConfig& config,
    std::unique_ptr<CrudClient<HsmNode>> client) :
    CrudService<HsmNode>(std::move(client)), m_config(config)
{
    LOG_INFO("Creating HsmNodeService");
}

HsmNodeService::~HsmNodeService()
{
    LOG_INFO("Destroying HsmNodeService");
}

HsmNodeService::Ptr HsmNodeService::create(
    const HsmNodeServiceConfig& config, KeyValueStoreClient* client)
{
    auto adapter = std::make_unique<HsmNodeJsonAdapter>();

    KeyValueCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<KeyValueCrudClient<HsmNode>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<HsmNodeService>(config, std::move(crud_client));
}

HsmNodeService::Ptr HsmNodeService::create(
    const HsmNodeServiceConfig& config, HttpClient* client)
{
    auto adapter = std::make_unique<HsmNodeJsonAdapter>();

    HttpCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<HttpCrudClient<HsmNode>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<HsmNodeService>(config, std::move(crud_client));
}

}  // namespace hestia