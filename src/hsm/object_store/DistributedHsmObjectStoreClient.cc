#include "DistributedHsmObjectStoreClient.h"

#include "HsmObjectStoreClientFactory.h"
#include "HsmObjectStoreClientManager.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "Logger.h"

#include "ErrorUtils.h"

#include "HttpClient.h"
#include <cassert>

namespace hestia {
DistributedHsmObjectStoreClient::DistributedHsmObjectStoreClient(
    std::unique_ptr<HsmObjectStoreClientManager> client_manager,
    HttpClient* http_client) :
    HsmObjectStoreClient(),
    m_http_client(std::move(http_client)),
    m_client_manager(std::move(client_manager))
{
}

DistributedHsmObjectStoreClient::~DistributedHsmObjectStoreClient() {}

DistributedHsmObjectStoreClient::Ptr DistributedHsmObjectStoreClient::create(
    HttpClient* http_client,
    const std::vector<std::filesystem::path>& plugin_paths)
{
    auto plugin_handler =
        std::make_unique<ObjectStorePluginHandler>(plugin_paths);
    auto client_factory = std::make_unique<HsmObjectStoreClientFactory>(
        std::move(plugin_handler));
    auto client_manager = std::make_unique<HsmObjectStoreClientManager>(
        std::move(client_factory));
    return std::make_unique<DistributedHsmObjectStoreClient>(
        std::move(client_manager), std::move(http_client));
}

void DistributedHsmObjectStoreClient::do_initialize(
    const std::string& cache_path, DistributedHsmService* hsm_service)
{
    m_hsm_service = hsm_service;

    const auto current_user_id =
        m_hsm_service->get_user_service()->get_current_user().get_primary_key();

    auto response =
        m_hsm_service->get_hsm_service()
            ->get_service(HsmItem::Type::TIER)
            ->make_request(CrudRequest{
                CrudQuery{CrudQuery::OutputFormat::ITEM}, current_user_id});

    std::vector<StorageTier> tiers;
    for (const auto& item : response->items()) {
        const auto tier = dynamic_cast<const StorageTier*>(item.get());
        LOG_INFO(
            "Adding tier: " << std::to_string(tier->id_uint())
                            << " with id: " << tier->get_primary_key());
        tiers.emplace_back(*tier);
    }

    m_client_manager->setup_clients(
        cache_path, m_hsm_service->get_self_config().m_self.get_primary_key(),
        tiers, m_hsm_service->get_backends());
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::do_remote_get(
    const HsmObjectStoreRequest& request, Stream* stream) const
{
    auto response = HsmObjectStoreResponse::create(request, "");

    if (m_http_client == nullptr) {
        const std::string message =
            "Object store client needs to hit remote but has no http client - possible config issue.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    if (m_hsm_service->get_self_config().m_is_server) {
        const std::string message =
            "Attempting remote get from a Server-Worker. Backends have been misconfigured.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    auto controller_endpoint =
        m_hsm_service->get_self_config().m_controller_address;

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    action.set_subject_key(request.object().get_primary_key());
    action.set_size(request.object().size());
    action.set_source_tier(request.source_tier());

    const auto path = controller_endpoint + "/api/v1/"
                      + hestia::HsmItem::hsm_action_name + "s";

    HttpRequest http_request(path, HttpRequest::Method::GET);
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    http_request.get_header().set_items(flat_dict);
    auto get_response = m_http_client->make_request(http_request, stream);

    if (get_response->error()) {
        LOG_ERROR("Failed in remote get request: " << get_response->message());
        response->on_error(
            {HsmObjectStoreErrorCode::ERROR, get_response->message()});
        return response;
    }
    return response;
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::do_remote_put(
    const HsmObjectStoreRequest& request, Stream* stream) const
{
    auto response = HsmObjectStoreResponse::create(request, "");

    if (m_http_client == nullptr) {
        const std::string message =
            "Object store client needs to hit remote but has no http client - possible config issue.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    if (m_hsm_service->get_self_config().m_is_server) {
        const std::string message =
            "Attempting remote put from a Server-Worker. Backends have been misconfigured.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    auto controller_endpoint =
        m_hsm_service->get_self_config().m_controller_address;

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    action.set_subject_key(request.object().get_primary_key());
    action.set_size(request.object().size());
    action.set_target_tier(request.target_tier());

    const auto path = controller_endpoint + "/api/v1/"
                      + hestia::HsmItem::hsm_action_name + "s";

    HttpRequest http_request(path, HttpRequest::Method::PUT);
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    http_request.get_header().set_items(flat_dict);
    auto put_response = m_http_client->make_request(http_request, stream);

    if (put_response->error()) {
        LOG_ERROR("Failed in remote put request: " << put_response->message());
        response->on_error(
            {HsmObjectStoreErrorCode::ERROR, put_response->message()});
        return response;
    }

    if (auto location = put_response->header().get_item("location");
        !location.empty()) {
        response->object().set_location(location);
    }
    else {
        response->object().set_location(controller_endpoint);
    }
    LOG_INFO(
        "Remote PUT complete ok to location: "
        << response->object().get_location());
    return response;
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::do_local_op(
    const HsmObjectStoreRequest& request, Stream* stream, uint8_t tier) const
{
    if (auto hsm_client = m_client_manager->get_hsm_client(tier);
        hsm_client != nullptr) {
        return hsm_client->make_request(request, stream);
    }
    else {
        auto client = m_client_manager->get_client(tier);
        assert(client != nullptr);
        auto response = client->make_request(
            HsmObjectStoreRequest::to_base_request(request), stream);
        return HsmObjectStoreResponse::create(request, std::move(response));
    }
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request, Stream* stream) const noexcept
{
    const auto current_user_id =
        m_hsm_service->get_user_service()->get_current_user().get_primary_key();

    if (request.method() == HsmObjectStoreRequestMethod::GET
        || request.method() == HsmObjectStoreRequestMethod::EXISTS) {
        if (m_client_manager->has_client(request.source_tier())) {
            LOG_INFO("Found local client for source tier");
            return do_local_op(request, stream, request.source_tier());
        }
        else if (request.method() == HsmObjectStoreRequestMethod::GET) {
            LOG_INFO(
                "Did not find client for source tier "
                + std::to_string(request.source_tier())
                + "- creating remote get");
            return do_remote_get(request, stream);
        }
    }
    else if (
        request.method() == HsmObjectStoreRequestMethod::PUT
        || request.method() == HsmObjectStoreRequestMethod::REMOVE) {
        if (m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Found local client for target tier");
            return do_local_op(request, stream, request.target_tier());
        }
        else if (request.method() == HsmObjectStoreRequestMethod::PUT) {
            LOG_INFO(
                "Did not find client for target tier "
                + std::to_string(request.target_tier())
                + " - creating remote put");
            return do_remote_put(request, stream);
        }
    }
    else if (
        request.method() == HsmObjectStoreRequestMethod::COPY
        || request.method() == HsmObjectStoreRequestMethod::MOVE) {
        if (m_client_manager->has_client(request.source_tier())
            && m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Both clients for COPY/MOVE found");
            if (m_client_manager->have_same_client_types(
                    request.source_tier(), request.target_tier())) {
                LOG_INFO("Same clients handling COPY/MOVE");
                auto client =
                    m_client_manager->get_hsm_client(request.target_tier());
                return client->make_request(request);
            }
            else {
                LOG_INFO("Doing COPY/MOVE between different clients");
                // copy between clients
                return nullptr;
            }
        }
        else if (m_client_manager->has_client(request.source_tier())) {
            LOG_INFO("Only have source client for COPY/MOVE");
            // put to target tier via dist hsm service
            return nullptr;
        }
        else if (m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Only have target client for COPY/MOVE");
            // pull from source tier via dist hsm service
            return nullptr;
        }
        else {
            LOG_INFO("Don't have local clients for COPY/MOVE - try remote");
            // request move on different hsm service node
            return nullptr;
        }
    }
    LOG_INFO("Method not handled by dist object store client");
    return nullptr;
}

bool DistributedHsmObjectStoreClient::is_controller_node() const
{
    return m_hsm_service->get_self_config().m_self.is_controller();
}

}  // namespace hestia
