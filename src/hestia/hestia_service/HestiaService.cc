#include "HestiaService.h"

#include "HttpClient.h"
#include "KeyValueStoreClient.h"

#include "Logger.h"

namespace hestia {

HestiaService::HestiaService(
    HestiaServiceConfig config,
    HsmService* hsm_service,
    std::unique_ptr<HestiaNodeService> node_service) :
    m_config(config),
    m_hsm_service(hsm_service),
    m_node_service(std::move(node_service))
{
}

HsmService* HestiaService::get_hsm_service()
{
    return m_hsm_service;
}

void HestiaService::get(std::vector<HestiaNode>& nodes) const
{
    LOG_INFO("Calling Node service multi get");

    const auto response = m_node_service->make_request(CrudMethod::MULTI_GET);
    nodes               = response->items();

    LOG_INFO("Finished Node service multi get");
}

void HestiaService::put(const HestiaNode& node) const
{
    LOG_INFO("Calling Node service put");

    const auto response = m_node_service->make_request({node, CrudMethod::PUT});
    (void)response;

    LOG_INFO("Finished Node service put");
}
}  // namespace hestia