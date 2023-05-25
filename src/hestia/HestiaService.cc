#include "HestiaService.h"

#include "HttpClient.h"
#include "KeyValueStoreClient.h"

namespace hestia {

HestiaService::HestiaService(
    HestiaServiceConfig config,
    HsmService* hsm_service,
    std::unique_ptr<KeyValueStoreClient> kv_store,
    std::unique_ptr<HttpClient> http_client) :
    m_config(config),
    m_hsm_service(hsm_service),
    m_kv_store(std::move(kv_store)),
    m_client(std::move(http_client))
{
}

HsmService* HestiaService::get_hsm_service()
{
    return m_hsm_service;
}

const std::vector<HestiaNode> HestiaService::get_nodes() const
{
    return m_nodes;
}

const std::vector<ObjectStoreBackend> HestiaService::get_backends() const
{
    return m_backends;
}
}  // namespace hestia