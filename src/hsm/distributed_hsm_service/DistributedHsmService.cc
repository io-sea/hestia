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

DistributedHsmService::Ptr DistributedHsmService::create(
    DistributedHsmServiceConfig config,
    HsmService* hsm_service,
    KeyValueStoreClient* client)
{
    HsmNodeServiceConfig node_service_config;
    node_service_config.m_global_prefix = config.m_app_name;
    auto node_service = HsmNodeService::create(node_service_config, client);

    auto service = std::make_unique<DistributedHsmService>(
        config, hsm_service, std::move(node_service));

    service->register_self();
    return service;
}

DistributedHsmService::Ptr DistributedHsmService::create(
    DistributedHsmServiceConfig config,
    HsmService* hsm_service,
    HttpClient* client)
{
    HsmNodeServiceConfig node_service_config;
    node_service_config.m_global_prefix = config.m_app_name;
    node_service_config.m_endpoint = config.m_controller_address + "/api/v1";
    auto node_service = HsmNodeService::create(node_service_config, client);

    auto service = std::make_unique<DistributedHsmService>(
        config, hsm_service, std::move(node_service));

    service->register_self();
    return service;
}

void DistributedHsmService::register_self()
{
    // Check if the node exists already
    Metadata query;
    query.set_item("tag", m_config.m_self.m_tag);
    LOG_INFO(
        "Checking for pre-registered endpoint with tag: "
        << m_config.m_self.m_tag);
    auto response = m_node_service->make_request(query);
    if (!response->items().empty()) {
        LOG_INFO("Found endpoint with id: " << response->items()[0].id());
        m_config.m_self.m_id = response->items()[0].id();
    }
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

    CrudRequest<HsmNode> request(node, CrudMethod::PUT);
    if (node.m_id.empty() && m_config.m_self.m_is_controller) {
        request.set_generate_id(true);
    }
    const auto response = m_node_service->make_request(request);
    (void)response;

    LOG_INFO("Finished Node service put");
}
}  // namespace hestia