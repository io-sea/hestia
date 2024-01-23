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
    return m_config.m_self.backends();
}

CrudResponse::Ptr DistributedHsmService::make_request(
    const CrudRequest& request, const std::string& type) const noexcept
{
    return m_hsm_service->make_request(request, type);
}

void on_exception(
    const HsmActionRequest& req,
    DistributedHsmService::actionCompletionFunc completion_func,
    const std::string& msg)
{
    LOG_ERROR(msg);
    auto response = HsmActionResponse::create(req, req.get_action());
    response->on_error({HsmActionErrorCode::STL_EXCEPTION, msg});
    completion_func(std::move(response));
}

void on_not_found(
    const HsmActionRequest& req,
    DistributedHsmService::actionCompletionFunc completion_func,
    const std::string& msg)
{
    LOG_ERROR(msg);
    auto response = HsmActionResponse::create(req, req.get_action());
    response->on_error({HsmActionErrorCode::ITEM_NOT_FOUND, msg});
    completion_func(std::move(response));
}

void DistributedHsmService::do_hsm_action(
    const HsmActionRequest& request,
    Stream* stream,
    actionCompletionFunc completion_func,
    actionProgressFunc progress_func) const noexcept
{
    if (!m_config.m_self.is_controller()) {
        LOG_INFO("In worker mode - running hsm service directly");
        m_hsm_service->do_hsm_action(
            request, stream, completion_func, progress_func);
        return;
    }

    // Check if we have a backend locally
    auto tier_id = request.is_put_data_action() ? request.target_tier_id() :
                                                  request.source_tier_id();
    uint8_t tier_priority{0};
    if (tier_id.empty()) {
        tier_priority = request.is_put_data_action() ? request.target_tier() :
                                                       request.source_tier();
        tier_id       = m_hsm_service->get_tier_id(tier_priority);
    }
    if (tier_id.empty()) {
        on_not_found(
            request, completion_func,
            "No tier found in system for priority "
                + std::to_string(tier_priority));
        return;
    }

    // Check for matching backend
    for (const auto& backend : get_backends()) {
        if (backend.has_tier(tier_id)) {
            LOG_INFO(
                "Controller has first requested backend locally - using it.");
            m_hsm_service->do_hsm_action(
                request, stream, completion_func, progress_func);
            return;
        }
    }

    // If we are copy or move check if the target tier is local
    if (request.is_copy_or_move_data_action()) {
        auto secondary_tier_id = request.target_tier_id();
        if (secondary_tier_id.empty()) {
            secondary_tier_id =
                m_hsm_service->get_tier_id(request.target_tier());
        }
        if (secondary_tier_id.empty()) {
            on_not_found(
                request, completion_func,
                "No tier found in system for priority "
                    + std::to_string(request.target_tier()));
            return;
        }
        for (const auto& backend : get_backends()) {
            if (backend.has_tier(secondary_tier_id)) {
                LOG_INFO(
                    "Controller has second requested backend locally - using it.");
                m_hsm_service->do_hsm_action(
                    request, stream, completion_func, progress_func);
                return;
            }
        }
    }

    // No local backends found - redirect to remote
    std::string node_address;
    std::string preferred_node_address =
        request.get_action().get_preffered_node_address();
    try {
        node_address = get_backend_address(
            tier_id, preferred_node_address, request.get_interface());
    }
    catch (const std::exception& e) {
        const std::string msg =
            SOURCE_LOC() + " | Exception getting backend address: " + e.what();
        on_exception(request, completion_func, msg);
        return;
    }

    if (node_address.empty()) {
        const std::string msg = "No backend found for tier: " + tier_id;
        on_not_found(request, completion_func, msg);
    }
    else {
        LOG_INFO("Redirecting to: " + node_address);
        auto response =
            HsmActionResponse::create(request, request.get_action());
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

bool DistributedHsmService::is_controller() const
{
    return get_controller_address().empty();
}

void DistributedHsmService::register_backends()
{
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
    const std::string& tier_id,
    const std::string& preferred_address,
    HsmNodeInterface::Type interface) const
{
    auto tier_service = m_hsm_service->get_service(HsmItem::Type::TIER);

    CrudQuery query(CrudIdentifier(tier_id), CrudQuery::BodyFormat::ITEM);

    auto get_response = tier_service->make_request(CrudRequest{
        CrudMethod::READ, query, m_user_service->get_current_user_context()});
    if (!get_response->ok()) {
        throw std::runtime_error(
            SOURCE_LOC() + " | Failed to check for tier in backend search"
            + get_response->get_error().to_string());
    }

    if (!get_response->found()) {
        throw std::runtime_error(
            SOURCE_LOC() + " | Didn't find tier with id " + tier_id
            + " in backend search");
    }

    auto tier = get_response->get_item_as<StorageTier>();
    if (tier->get_backends().empty()) {
        LOG_INFO("No backends found for tier: " + tier_id);
        return {};
    }

    std::string first_address;
    for (const auto& backend : tier->get_backends()) {
        const auto node_id = backend.get_node_id();
        LOG_INFO("Checking node id: " << node_id << " for backend");

        const auto& [address, port] = get_node_address(node_id, interface);
        auto node_address           = address;
        if (port > 0) {
            node_address += ":" + std::to_string(port);
        }
        if (first_address.empty()) {
            first_address = node_address;
        }
        if (preferred_address.empty() || preferred_address == address
            || preferred_address == node_address) {
            if (preferred_address == address
                || preferred_address == node_address) {
                LOG_INFO("Using preferred address: " + address);
            }
            else {
                LOG_INFO(
                    "No preferred address set, using first address found: "
                    + first_address);
            }
            return node_address;
        }
    }
    if (preferred_address.empty()) {
        LOG_INFO(
            "No preferred address given - using first address found: "
            + first_address);
    }
    else {
        LOG_INFO(
            "Could not find preferred address " + preferred_address
            + " - using first address found: " + first_address);
    }
    return first_address;
}

std::pair<std::string, unsigned> DistributedHsmService::get_node_address(
    const std::string& node_id, HsmNodeInterface::Type interface_type) const
{
    auto node_service      = m_hsm_service->get_service(HsmItem::Type::NODE);
    auto node_get_response = node_service->make_request(CrudRequest{
        CrudMethod::READ,
        CrudQuery{
            node_id,
            {CrudQuery::BodyFormat::ITEM, CrudQuery::ChildFormat::NONE}},
        m_user_service->get_current_user_context()});
    if (!node_get_response->ok()) {
        throw std::runtime_error(
            SOURCE_LOC() + " | Failed to check for node in backend search"
            + node_get_response->get_error().to_string());
    }
    if (!node_get_response->found()) {
        throw std::runtime_error(
            SOURCE_LOC() + " | Didn't find node " + node_id
            + " in backend search");
    }

    auto node = node_get_response->get_item_as<HsmNode>();
    for (const auto& interface : node->get_interfaces()) {
        if (interface.get_type() == interface_type) {
            return {node->host(), interface.get_port()};
        }
    }
    throw std::runtime_error(
        SOURCE_LOC() + " | Didn't find requested interface for " + node_id
        + " in backend search");
}

HsmService* DistributedHsmService::get_hsm_service()
{
    return m_hsm_service.get();
}

}  // namespace hestia
