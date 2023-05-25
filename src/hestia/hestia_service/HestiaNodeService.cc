#include "HestiaNodeService.h"

#include "HestiaNodeAdapter.h"
#include "JsonUtils.h"
#include "KeyValueStoreClient.h"
#include "Logger.h"

namespace hestia {
HestiaNodeService::HestiaNodeService(
    const HestiaNodeServiceConfig& config,
    KeyValueStoreClient* kv_store_client,
    HttpClient* http_client) :
    KeyValueCrudService<HestiaNode>(
        {"hestia", "node", config.m_endpoint}, kv_store_client),
    m_http_client(http_client)
{
    LOG_INFO("Creating HestiaNodeService");
    (void)m_http_client;
}

HestiaNodeService::~HestiaNodeService()
{
    LOG_INFO("Destroying HestiaNodeService");
}

void HestiaNodeService::to_string(
    const HestiaNode& item, std::string& output) const
{
    HestiaNodeAdapter::to_json(item, output);
}

void HestiaNodeService::from_string(
    const std::string& output, HestiaNode& item) const
{
    HestiaNodeAdapter::from_json(output, item);
}


}  // namespace hestia