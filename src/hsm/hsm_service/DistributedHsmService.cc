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
            CrudMethod::READ, CrudQuery{CrudQuery::BodyFormat::ITEM},
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

CrudResponse::Ptr DistributedHsmService::make_request(
    const CrudRequest& request, const std::string& type) const noexcept
{
    return m_hsm_service->make_request(request, type);
}

void DistributedHsmService::do_hsm_action(
    const HsmActionRequest& request,
    Stream* stream,
    actionCompletionFunc completion_func,
    actionProgressFunc progress_func) const noexcept
{
    if (!m_config.m_self.is_controller()) {
        m_hsm_service->do_hsm_action(
            request, stream, completion_func, progress_func);
        return;
    }

    const auto tier = request.is_put_data_action() ?
                          std::to_string(request.target_tier()) :
                          std::to_string(request.source_tier());
    for (const auto& backend : get_backends()) {
        if (backend.has_tier_name(tier)) {
            LOG_INFO(
                "Controller has first requested backend locally - using it.");
            m_hsm_service->do_hsm_action(
                request, stream, completion_func, progress_func);
            return;
        }
    }

    if (request.is_copy_or_move_data_action()) {
        const auto secondary_tier = std::to_string(request.target_tier());
        for (const auto& backend : get_backends()) {
            if (backend.has_tier_name(secondary_tier)) {
                LOG_INFO(
                    "Controller has second requested backend locally - using it.");
                m_hsm_service->do_hsm_action(
                    request, stream, completion_func, progress_func);
                return;
            }
        }
    }

    // No local backends found - redirect to remote
    auto response = HsmActionResponse::create(request, request.get_action());

    const auto remote_tier = request.is_put_data_action() ?
                                 request.target_tier() :
                                 request.source_tier();
    std::string node_address;
    try {
        node_address = get_backend_address(remote_tier);
    }
    catch (const std::exception& e) {
        const std::string msg =
            SOURCE_LOC() + " | Exception getting backend address: " + e.what();
        LOG_ERROR(msg);
        response->on_error({HsmActionErrorCode::STL_EXCEPTION, msg});
        completion_func(std::move(response));
    }

    if (node_address.empty()) {
        const std::string msg =
            "No backend found for tier: " + std::to_string(remote_tier);
        LOG_ERROR(msg);
        response->on_error({HsmActionErrorCode::ITEM_NOT_FOUND, msg});
        completion_func(std::move(response));
    }
    else {
        LOG_INFO("Redirecting to: " + node_address);
        response->set_redirect_location(node_address);
        completion_func(std::move(response));
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
        CrudMethod::READ, CrudQuery(id, CrudQuery::BodyFormat::ITEM),
        m_user_service->get_current_user_context()});
    if (!get_response->ok()) {
        throw std::runtime_error(
            "Failed to check for pre-existing tag: "
            + get_response->get_error().to_string());
    }

    if (!get_response->found()) {
        LOG_INFO("Pre-existing endpoint not found - will request new one.");

        auto create_response = node_service->make_request(TypedCrudRequest{
            CrudMethod::CREATE,
            m_config.m_self,
            {CrudQuery::BodyFormat::ITEM},
            m_user_service->get_current_user_context()});
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

const std::string& DistributedHsmService::get_controller_address() const
{
    return get_self_config().m_controller_address;
}

bool DistributedHsmService::is_server() const
{
    return get_self_config().m_is_server;
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
                CrudMethod::CREATE, backend, {},
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
        CrudMethod::READ, CrudQuery(id, CrudQuery::BodyFormat::ITEM),
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
        CrudMethod::READ, CrudQuery{tier_id, CrudQuery::BodyFormat::ITEM},
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
        CrudMethod::READ, CrudQuery{node_id, CrudQuery::BodyFormat::ITEM},
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
