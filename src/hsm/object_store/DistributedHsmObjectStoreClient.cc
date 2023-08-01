#include "DistributedHsmObjectStoreClient.h"

#include "HsmObjectStoreClientFactory.h"
#include "HsmObjectStoreClientManager.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "Logger.h"

#include "HttpClient.h"

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
        cache_path, m_hsm_service->get_self_config().m_self.backends(), tiers);
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request, Stream* stream) const noexcept
{
    const auto current_user_id =
        m_hsm_service->get_user_service()->get_current_user().get_primary_key();

    if (request.method() == HsmObjectStoreRequestMethod::GET
        || request.method() == HsmObjectStoreRequestMethod::EXISTS) {
        if (m_client_manager->has_client(request.source_tier())) {
            if (m_client_manager->is_hsm_client(request.source_tier())) {
                auto client =
                    m_client_manager->get_hsm_client(request.source_tier());
                return client->make_request(request, stream);
            }
            else {
                auto client =
                    m_client_manager->get_client(request.source_tier());
                auto response = client->make_request(
                    HsmObjectStoreRequest::to_base_request(request), stream);
                return HsmObjectStoreResponse::create(
                    request, std::move(response));
            }
        }
        else {
            auto controller_endpoint =
                m_hsm_service->get_self_config().m_controller_address;

            HsmAction action(
                HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
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
            auto get_response =
                m_http_client->make_request(http_request, stream);

            if (get_response->error()) {
                LOG_ERROR(
                    "Failed in remote get request: "
                    << get_response->message());

                auto response = HsmObjectStoreResponse::create(request);
                response->on_error(
                    {HsmObjectStoreErrorCode::ERROR, get_response->message()});
                return response;
            }
            if (stream != nullptr && stream->has_content()) {
                auto stream_state = stream->flush();
                if (!stream_state.ok()) {
                    auto response = HsmObjectStoreResponse::create(request);
                    response->on_error(
                        {HsmObjectStoreErrorCode::ERROR,
                         "Failed in stream flush: "
                             + stream_state.to_string()});
                    return response;
                }
            }
            return HsmObjectStoreResponse::create(request);
        }
    }
    else if (
        request.method() == HsmObjectStoreRequestMethod::PUT
        || request.method() == HsmObjectStoreRequestMethod::REMOVE) {
        if (m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Found client for target tier");
            if (m_client_manager->is_hsm_client(request.target_tier())) {
                auto client =
                    m_client_manager->get_hsm_client(request.target_tier());
                return client->make_request(request, stream);
            }
            else {
                auto client =
                    m_client_manager->get_client(request.target_tier());
                auto response = client->make_request(
                    HsmObjectStoreRequest::to_base_request(request), stream);
                return HsmObjectStoreResponse::create(
                    request, std::move(response));
            }
        }
        else {
            LOG_INFO(
                "Did not find client for target tier - creating remote action");

            auto controller_endpoint =
                m_hsm_service->get_self_config().m_controller_address;

            HsmAction action(
                HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
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
            auto put_response =
                m_http_client->make_request(http_request, stream);

            if (put_response->error()) {
                LOG_ERROR(
                    "Failed in remote put request: "
                    << put_response->message());

                auto response = HsmObjectStoreResponse::create(request);
                response->on_error(
                    {HsmObjectStoreErrorCode::ERROR, put_response->message()});
                return response;
            }
            if (stream != nullptr && stream->has_content()) {
                auto stream_state = stream->flush();
                if (!stream_state.ok()) {
                    auto response = HsmObjectStoreResponse::create(request);
                    response->on_error(
                        {HsmObjectStoreErrorCode::ERROR,
                         "Failed in stream flush: "
                             + stream_state.to_string()});
                    return response;
                }
            }
            return HsmObjectStoreResponse::create(request);
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
}  // namespace hestia
