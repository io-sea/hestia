#include "DistributedHsmService.h"

#include "HttpClient.h"
#include "KeyValueStoreClient.h"

#include "Logger.h"

namespace hestia {

DistributedHsmService::DistributedHsmService(
    DistributedHsmServiceConfig config,
    HsmService* hsm_service,
    std::unique_ptr<HsmNodeService> node_service) :
    m_config(config),
    m_hsm_service(hsm_service),
    m_node_service(std::move(node_service))
{
}

void DistributedHsmService::register_self()
{
    put(m_config.m_self);
}

HsmService* DistributedHsmService::get_hsm_service()
{
    return m_hsm_service;
}

void DistributedHsmService::get(std::vector<HsmNode>& nodes) const
{
    LOG_INFO("Calling Node service multi get");

    const auto response = m_node_service->make_request(CrudMethod::MULTI_GET);
    nodes               = response->items();

    LOG_INFO("Finished Node service multi get");
}

void DistributedHsmService::put(const HsmNode& node) const
{
    LOG_INFO("Calling Node service put");

    const auto response = m_node_service->make_request({node, CrudMethod::PUT});
    (void)response;

    LOG_INFO("Finished Node service put");
}
}  // namespace hestia