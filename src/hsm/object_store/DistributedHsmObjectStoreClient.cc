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
        LOG_INFO("Adding tier: " << std::to_string(tier->id_uint()));
        tiers.emplace_back(*tier);
    }

    m_client_manager->setup_clients(
        cache_path, m_hsm_service->get_backends(), tiers);
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::do_remote_get(
    const HsmObjectStoreRequest& request, Stream* stream) const
{
    auto response = HsmObjectStoreResponse::create(request, "");

    if (is_controller_node()) {
        const auto current_user_id = m_hsm_service->get_user_service()
                                         ->get_current_user()
                                         .get_primary_key();

        auto object_service = m_hsm_service->get_hsm_service()->get_service(
            HsmItem::Type::OBJECT);

        auto obj_get_response = object_service->make_request(CrudRequest{
            CrudQuery{
                request.object().get_primary_key(),
                CrudQuery::OutputFormat::ITEM},
            current_user_id});
        if (!obj_get_response->ok()) {
            LOG_ERROR(
                "Failed in object get request: "
                << obj_get_response->get_error().to_string());
            response->on_error(
                {HsmObjectStoreErrorCode::ERROR,
                 obj_get_response->get_error().to_string()});
            return response;
        }
    }
    else {
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
            LOG_ERROR(
                "Failed in remote get request: " << get_response->message());
            response->on_error(
                {HsmObjectStoreErrorCode::ERROR, get_response->message()});
            return response;
        }
        if (stream != nullptr && stream->has_content()) {
            auto stream_state = stream->flush();
            if (!stream_state.ok()) {
                response->on_error(
                    {HsmObjectStoreErrorCode::ERROR,
                     "Failed in stream flush: " + stream_state.to_string()});
                return response;
            }
        }
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

    if (is_controller_node()) {
        const auto current_user_id = m_hsm_service->get_user_service()
                                         ->get_current_user()
                                         .get_primary_key();
        const auto backend =
            m_client_manager->get_backend(request.target_tier());
        if (backend == ObjectStoreBackend::Type::UNKNOWN) {
            const std::string msg = "No backend registered for requested tier: "
                                    + std::to_string(request.target_tier());
            LOG_ERROR(msg);
            response->on_error({HsmObjectStoreErrorCode::ERROR, msg});
            return response;
        }

        auto node_service =
            m_hsm_service->get_hsm_service()->get_service(HsmItem::Type::NODE);

        ObjectStoreBackend::Type_enum_string_converter backend_serializer;
        backend_serializer.init();
        Map filter;
        filter.set_item("backend", backend_serializer.to_string(backend));

        CrudQuery query(
            filter, CrudQuery::Format::LIST, CrudQuery::OutputFormat::ID);
        auto node_list_response =
            node_service->make_request(CrudRequest{query, current_user_id});
        if (!node_list_response->ok()) {
            LOG_ERROR(
                "Failed in node list request: "
                << node_list_response->get_error().to_string());
            response->on_error(
                {HsmObjectStoreErrorCode::ERROR,
                 node_list_response->get_error().to_string()});
            return response;
        }

        if (node_list_response->items().empty()) {
            const std::string msg =
                "Failed to find any online nodes in DB for requested backend: "
                + backend_serializer.to_string(backend);
            LOG_ERROR(msg);
            response->on_error({HsmObjectStoreErrorCode::ERROR, msg});
            return response;
        }

        std::string location;
        for (const auto& node_item : node_list_response->items()) {
            const auto node = dynamic_cast<HsmNode*>(node_item.get());
            const std::string node_address =
                node->host() + ":" + std::to_string(node->port());
            const auto path = node_address + "/api/v1/ping";
            HttpRequest http_request(path, HttpRequest::Method::HEAD);
            auto ping_response = m_http_client->make_request(http_request);

            if (ping_response->error()) {
                LOG_INFO("Endpoint at " + node_address + " not online");
                continue;
            }
            location = node_address;
            break;
        }
        if (location.empty()) {
            const std::string msg =
                "Failed to find any online nodes for requested backend: "
                + backend_serializer.to_string(backend);
            LOG_ERROR(msg);
            response->on_error({HsmObjectStoreErrorCode::ERROR, msg});
            return response;
        }
        response->object().set_location(location);
        return response;
    }
    else {
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
            LOG_ERROR(
                "Failed in remote put request: " << put_response->message());
            response->on_error(
                {HsmObjectStoreErrorCode::ERROR, put_response->message()});
            return response;
        }
        if (stream != nullptr && stream->has_content()) {
            auto stream_state = stream->flush();
            if (!stream_state.ok()) {
                response->on_error(
                    {HsmObjectStoreErrorCode::ERROR,
                     "Failed in stream flush: " + stream_state.to_string()});
                return response;
            }
        }
    }
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
                "Did not find client for source tier - creating remote action");
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
                "Did not find client for target tier - creating remote action");
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
