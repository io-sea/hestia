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

HsmActionResponse::Ptr DistributedHsmService::make_request(
    const HsmActionRequest& request) const noexcept
{
    return m_hsm_service->make_request(request);
}

void DistributedHsmService::do_data_io_action(
    const HsmActionRequest& request,
    Stream* stream,
    dataIoCompletionFunc completion_func) const
{

    if (m_config.m_self.is_controller()) {

        std::string tier_name;
        if (request.get_action().get_action() == HsmAction::Action::PUT_DATA) {
            tier_name = std::to_string(request.target_tier());
        }
        else {
            tier_name = std::to_string(request.source_tier());
        }

        for (const auto& backend : get_backends()) {
            if (backend.has_tier_name(tier_name)) {
                LOG_INFO("Controller has backend locally - using that.");
                m_hsm_service->do_data_io_action(
                    request, stream, completion_func);
                return;
            }
        }

        if (request.get_action().get_action() == HsmAction::Action::PUT_DATA) {
            auto response =
                HsmActionResponse::create(request, request.get_action());

            auto node_address = get_backend_address(request.target_tier());
            if (node_address.empty()) {
                const std::string message =
                    "No backend found for tier: "
                    + std::to_string(request.target_tier());
                LOG_ERROR(message);
                response->on_error(
                    {HsmActionErrorCode::ITEM_NOT_FOUND, message});
                completion_func(std::move(response));
            }
            else {
                LOG_INFO("Redirecting to: " + node_address);
                response->set_redirect_location(node_address);
                completion_func(std::move(response));
            }
        }
        else if (
            request.get_action().get_action() == HsmAction::Action::GET_DATA) {
            auto response =
                HsmActionResponse::create(request, request.get_action());

            auto node_address = get_backend_address(request.source_tier());
            if (node_address.empty()) {
                const std::string message =
                    "No backend found for tier: "
                    + std::to_string(request.source_tier());
                LOG_ERROR(message);
                response->on_error(
                    {HsmActionErrorCode::ITEM_NOT_FOUND, message});
                completion_func(std::move(response));
            }
            else {
                LOG_INFO("Redirecting to: " + node_address);
                response->set_redirect_location(node_address);
                completion_func(std::move(response));
            }
        }
    }
    else {
        m_hsm_service->do_data_io_action(request, stream, completion_func);
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
        m_user_service->get_current_user_context()});
    if (!get_response->ok()) {
        throw std::runtime_error(
            "Failed to check for pre-existing tag: "
            + get_response->get_error().to_string());
    }

    if (!get_response->found()) {
        LOG_INFO("Pre-existing endpoint not found - will request new one.");

        auto create_response = node_service->make_request(TypedCrudRequest{
            CrudMethod::CREATE, m_config.m_self,
            m_user_service->get_current_user_context(),
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

        Dictionary dict;
        backend.serialize(dict);

        auto response =
            backend_service->make_request(TypedCrudRequest<ObjectStoreBackend>(
                CrudMethod::CREATE, backend,
                m_user_service->get_current_user_context()));
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
        m_user_service->get_current_user_context()});
    if (!get_response->ok()) {
        throw std::runtime_error(
            "Failed to update local node after registering backends: "
            + get_response->get_error().to_string());
    }
    m_config.m_self = *get_response->get_item_as<HsmNode>();

    LOG_INFO(
        "Have: " << m_config.m_self.backends().size()
                 << " backends after registration for node: "
                 << m_config.m_self.get_primary_key());

    if (!m_config.m_self.backends().empty()) {
        Dictionary dict;
        m_config.m_self.backends()[0].serialize(dict);
    }
}

std::string DistributedHsmService::get_backend_address(
    uint8_t tier_name, const std::string&) const
{
    auto tier_service = m_hsm_service->get_service(HsmItem::Type::TIER);
    CrudIdentifier tier_id(
        std::to_string(tier_name), CrudIdentifier::Type::NAME);
    auto get_response = tier_service->make_request(CrudRequest{
        CrudQuery{tier_id, CrudQuery::OutputFormat::ITEM},
        m_user_service->get_current_user_context()});
    if (!get_response->ok()) {
        throw std::runtime_error(
            "Failed to check for tier in backend search"
            + get_response->get_error().to_string());
    }

    if (!get_response->found()) {
        throw std::runtime_error(
            "Didn't find tier " + tier_id.get_name() + " in backend search");
    }

    auto tier = get_response->get_item_as<StorageTier>();
    if (tier->get_backends().empty()) {
        LOG_INFO("No backends found for tier: " + tier_id.get_name());
        return {};
    }
    const auto node_id = tier->get_backends()[0].get_node_id();
    LOG_INFO("Checking node id: " << node_id << " for backend");

    auto node_service      = m_hsm_service->get_service(HsmItem::Type::NODE);
    auto node_get_response = node_service->make_request(CrudRequest{
        CrudQuery{node_id, CrudQuery::OutputFormat::ITEM},
        m_user_service->get_current_user_context()});
    if (!node_get_response->ok()) {
        throw std::runtime_error(
            "Failed to check for node in backend search"
            + node_get_response->get_error().to_string());
    }

    if (!node_get_response->found()) {
        throw std::runtime_error(
            "Didn't find node " + node_id + " in backend search");
    }

    auto node = node_get_response->get_item_as<HsmNode>();
    LOG_INFO("Got node id: " << node->get_primary_key());
    LOG_INFO("Got node address: " << node->host());
    auto node_address = node->host();
    if (node->port() > 0) {
        node_address += ":" + std::to_string(node->port());
    }
    return node_address;
}

HsmService* DistributedHsmService::get_hsm_service()
{
    return m_hsm_service.get();
}

}  // namespace hestia