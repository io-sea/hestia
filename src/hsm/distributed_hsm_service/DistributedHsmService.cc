#include "DistributedHsmService.h"

#include "CrudServiceFactory.h"
#include "HsmService.h"
#include "HttpClient.h"
#include "KeyValueStoreClient.h"

#include "IdGenerator.h"
#include "TimeProvider.h"

#include "TypedCrudRequest.h"

#include "Logger.h"

namespace hestia {

DistributedHsmService::DistributedHsmService(
    DistributedHsmServiceConfig config,
    std::unique_ptr<HsmService> hsm_service,
    CrudService::Ptr node_service) :
    m_config(config),
    m_hsm_service(std::move(hsm_service)),
    m_node_service(std::move(node_service))
{
}

DistributedHsmService::Ptr DistributedHsmService::create(
    DistributedHsmServiceConfig config,
    std::unique_ptr<HsmService> hsm_service,
    CrudServiceBackend* backend,
    UserService* user_service)
{
    ServiceConfig service_config;
    service_config.m_endpoint = config.m_controller_address + "/api/v1";

    auto node_service = CrudServiceFactory<HsmNode>::create(
        service_config, backend, user_service);

    auto service = std::make_unique<DistributedHsmService>(
        config, std::move(hsm_service), std::move(node_service));
    return service;
}

DistributedHsmService::~DistributedHsmService() {}

const DistributedHsmServiceConfig& DistributedHsmService::get_self_config()
    const
{
    return m_config;
}

void DistributedHsmService::register_self()
{
    LOG_INFO(
        "Checking for pre-registered endpoint with name: "
        << m_config.m_self.name());

    CrudIdentifier id(m_config.m_self.name(), CrudIdentifier::Type::NAME);
    auto get_response = m_node_service->make_request(
        CrudRequest{CrudQuery(id, CrudQuery::OutputFormat::ITEM)});
    if (!get_response->ok()) {
        throw std::runtime_error(
            "Failed to check for pre-existing tag: "
            + get_response->get_error().to_string());
    }

    if (!get_response->found()) {
        LOG_INFO("Pre-existing endpoint not found - will request new one.");

        auto create_response = m_node_service->make_request(CrudRequest{
            CrudMethod::CREATE, {id}, {}, CrudQuery::OutputFormat::ITEM});
        if (!create_response->ok()) {
            LOG_ERROR(
                "Failed to register node: "
                + create_response->get_error().to_string());
            throw std::runtime_error(
                "Failed to register node: "
                + create_response->get_error().to_string());
        }
        m_config.m_self = *create_response->get_item_as<HsmNode>();
    }
    else {
        LOG_INFO("Found endpoint with id: " << get_response->get_item()->id());
        m_config.m_self = *get_response->get_item_as<HsmNode>();
    }
}

HsmService* DistributedHsmService::get_hsm_service()
{
    return m_hsm_service.get();
}

CrudService* DistributedHsmService::get_node_service()
{
    return m_node_service.get();
}

}  // namespace hestia