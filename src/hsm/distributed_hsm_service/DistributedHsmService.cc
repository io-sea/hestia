#include "DistributedHsmService.h"

#include "CrudServiceFactory.h"
#include "HsmService.h"
#include "HttpClient.h"
#include "KeyValueStoreClient.h"

#include "IdGenerator.h"
#include "StorageTier.h"
#include "TimeProvider.h"

#include "ErrorUtils.h"
#include "TypedCrudRequest.h"

#include "Logger.h"

namespace hestia {

DistributedHsmService::DistributedHsmService(
    DistributedHsmServiceConfig config,
    std::unique_ptr<HsmService> hsm_service,
    UserService* user_service) :
    m_config(config),
    m_hsm_service(std::move(hsm_service)),
    m_user_service(user_service)
{
    if (!m_config.m_backends.empty()) {
        LOG_INFO("Assigning tier ids to config backends from names");
        auto tier_service  = m_hsm_service->get_service(HsmItem::Type::TIER);
        auto tier_response = tier_service->make_request(CrudRequest{
            CrudQuery{CrudQuery::OutputFormat::ITEM},
            user_service->get_current_user().get_primary_key()});

        std::unordered_map<std::string, std::string> tier_ids;
        for (const auto& tier_item : tier_response->items()) {
            auto tier = dynamic_cast<StorageTier*>(tier_item.get());
            tier_ids[tier->name()] = tier->get_primary_key();
        }

        for (auto& backend : m_config.m_backends) {
            for (const auto& tier_name : backend.get_tier_names()) {
                const auto tier_id = tier_ids[tier_name];
                if (!tier_id.empty()) {
                    LOG_INFO(
                        "Adding tier with name: " + tier_name + " and id "
                        + tier_id + " to backend");
                    backend.add_tier_id(tier_ids[tier_name]);
                }
            }
        }

        if (!m_config.m_is_server) {
            // For clients with backends they are not stored in a db - so we can
            // give then any id format we like.
            std::size_t count{0};
            for (auto& backend : m_config.m_backends) {
                backend.init_id(std::to_string(count));
                count++;
            }
        }
    }
}

DistributedHsmService::Ptr DistributedHsmService::create(
    DistributedHsmServiceConfig config,
    std::unique_ptr<HsmService> hsm_service,
    UserService* user_service)
{
    ServiceConfig service_config;
    service_config.m_endpoint      = config.m_controller_address + "/api/v1";
    service_config.m_global_prefix = config.m_app_name;

    auto service = std::make_unique<DistributedHsmService>(
        config, std::move(hsm_service), user_service);
    return service;
}

DistributedHsmService::~DistributedHsmService() {}

const DistributedHsmServiceConfig& DistributedHsmService::get_self_config()
    const
{
    return m_config;
}

const std::vector<ObjectStoreBackend>& DistributedHsmService::get_backends()
    const
{
    if (m_config.m_is_server) {
        return m_config.m_self.backends();
    }
    else {
        return m_config.m_backends;
    }
}

UserService* DistributedHsmService::get_user_service()
{
    return m_user_service;
}

void DistributedHsmService::register_self()
{
    LOG_INFO(
        "Checking for pre-registered endpoint with name: "
        << m_config.m_self.name());

    auto node_service = m_hsm_service->get_service(HsmItem::Type::NODE);

    CrudIdentifier id(m_config.m_self.name(), CrudIdentifier::Type::NAME);
    auto get_response = node_service->make_request(CrudRequest{
        CrudQuery(id, CrudQuery::OutputFormat::ITEM),
        m_user_service->get_current_user().get_primary_key()});
    if (!get_response->ok()) {
        throw std::runtime_error(
            "Failed to check for pre-existing tag: "
            + get_response->get_error().to_string());
    }

    if (!get_response->found()) {
        LOG_INFO("Pre-existing endpoint not found - will request new one.");

        auto create_response = node_service->make_request(TypedCrudRequest{
            CrudMethod::CREATE, m_config.m_self,
            m_user_service->get_current_user().get_primary_key(),
            CrudQuery::OutputFormat::ITEM});
        if (!create_response->ok()) {
            LOG_ERROR(
                "Failed to register node: "
                + create_response->get_error().to_string());
            throw std::runtime_error(
                "Failed to register node: "
                + create_response->get_error().to_string());
        }
        m_config.m_self = *create_response->get_item_as<HsmNode>();

        register_backends();
    }
    else {
        LOG_INFO("Found endpoint with id: " << get_response->get_item()->id());
        m_config.m_self = *get_response->get_item_as<HsmNode>();
    }
}

void DistributedHsmService::register_backends()
{
    if (m_config.m_backends.empty()) {
        return;
    }

    auto backend_service =
        m_hsm_service->get_service(HsmItem::Type::OBJECT_STORE_BACKEND);

    for (auto backend : m_config.m_backends) {
        LOG_INFO(
            "Adding self id to backend: " << m_config.m_self.get_primary_key());
        backend.set_node_id(m_config.m_self.get_primary_key());
        auto response =
            backend_service->make_request(TypedCrudRequest<ObjectStoreBackend>(
                CrudMethod::CREATE, backend,
                m_user_service->get_current_user().get_primary_key()));
        if (!response->ok()) {
            throw std::runtime_error(
                "Failed to register backend: "
                + response->get_error().to_string());
        }
    }

    // Now the backends are registered - update the current node
    auto node_service = m_hsm_service->get_service(HsmItem::Type::NODE);

    CrudIdentifier id(m_config.m_self.get_primary_key());
    auto get_response = node_service->make_request(CrudRequest{
        CrudQuery(id, CrudQuery::OutputFormat::ITEM),
        m_user_service->get_current_user().get_primary_key()});
    if (!get_response->ok()) {
        throw std::runtime_error(
            "Failed to update local node after registering backends: "
            + get_response->get_error().to_string());
    }
    m_config.m_self = *get_response->get_item_as<HsmNode>();
}

HsmService* DistributedHsmService::get_hsm_service()
{
    return m_hsm_service.get();
}

}  // namespace hestia