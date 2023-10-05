#include "DistributedHsmObjectStoreClient.h"

#include "HsmObjectStoreClientFactory.h"
#include "HsmObjectStoreClientManager.h"

#include "DistributedHsmService.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "HsmService.h"
#include "Logger.h"

#include "ErrorUtils.h"

#include "HttpClient.h"
#include "S3Client.h"
#include <cassert>

namespace hestia {
DistributedHsmObjectStoreClient::DistributedHsmObjectStoreClient(
    std::unique_ptr<HsmObjectStoreClientManager> client_manager,
    HttpClient* http_client,
    S3Client* s3_client,
    DistributedHsmObjectStoreClientConfig config) :
    HsmObjectStoreClient(),
    m_http_client(http_client),
    m_s3_client(s3_client),
    m_client_manager(std::move(client_manager)),
    m_config(config)
{
}

DistributedHsmObjectStoreClient::~DistributedHsmObjectStoreClient() {}

DistributedHsmObjectStoreClient::Ptr DistributedHsmObjectStoreClient::create(
    HttpClient* http_client,
    S3Client* s3_client,
    const std::vector<std::filesystem::path>& plugin_paths)
{
    auto plugin_handler =
        std::make_unique<ObjectStorePluginHandler>(plugin_paths);
    auto client_factory = std::make_unique<HsmObjectStoreClientFactory>(
        std::move(plugin_handler));
    auto client_manager = std::make_unique<HsmObjectStoreClientManager>(
        std::move(client_factory));
    return std::make_unique<DistributedHsmObjectStoreClient>(
        std::move(client_manager), http_client, s3_client);
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
        m_s3_client, tiers, m_hsm_service->get_backends());
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
    action.set_primary_key(request.get_action_id());

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

    if (auto location = get_response->header().get_item("location");
        !location.empty()) {
        response->object().set_location(location);
    }
    else {
        response->object().set_location(controller_endpoint);
    }
    LOG_INFO(
        "Remote GET complete ok from location: "
        << response->object().get_location());

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
    action.set_size(stream->get_source_size());
    action.set_target_tier(request.target_tier());
    action.set_primary_key(request.get_action_id());

    const auto path = controller_endpoint + "/api/v1/"
                      + hestia::HsmItem::hsm_action_name + "s";

    HttpRequest http_request(path, HttpRequest::Method::PUT);
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    http_request.get_header().set_items(flat_dict);
    http_request.get_header().set_item(
        "content-length", std::to_string(stream->get_source_size()));
    http_request.get_header().set_item(
        "Authorisation",
        request.object().metadata().get_item("hestia-user_token"));

    const auto put_response = m_http_client->make_request(http_request, stream);
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

HsmObjectStoreResponse::Ptr
DistributedHsmObjectStoreClient::do_remote_copy_or_move(
    const HsmObjectStoreRequest& request, bool is_copy) const
{
    auto response = HsmObjectStoreResponse::create(request, "");
    response->set_is_handled_remote(true);

    if (m_http_client == nullptr) {
        const std::string message =
            "Object store client needs to hit remote but has no http client - possible config issue.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    if (m_hsm_service->get_self_config().m_is_server) {
        const std::string message =
            "Attempting remote copy/move from a Server-Worker. Backends have been misconfigured.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    auto controller_endpoint =
        m_hsm_service->get_self_config().m_controller_address;

    auto action_type =
        is_copy ? HsmAction::Action::COPY_DATA : HsmAction::Action::MOVE_DATA;

    HsmAction action(HsmItem::Type::OBJECT, action_type);
    action.set_subject_key(request.object().get_primary_key());
    action.set_target_tier(request.target_tier());
    action.set_source_tier(request.source_tier());
    action.set_primary_key(request.get_action_id());

    const auto path = controller_endpoint + "/api/v1/"
                      + hestia::HsmItem::hsm_action_name + "s";

    HttpRequest http_request(path, HttpRequest::Method::PUT);
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    http_request.get_header().set_items(flat_dict);
    http_request.get_header().set_item(
        "Authorisation",
        request.object().metadata().get_item("hestia-user_token"));

    const auto http_response = m_http_client->make_request(http_request);
    if (http_response->error()) {
        LOG_ERROR("Failed in remote put request: " << http_response->message());
        response->on_error(
            {HsmObjectStoreErrorCode::ERROR, http_response->message()});
        return response;
    }

    if (auto location = http_response->header().get_item("location");
        !location.empty()) {
        response->object().set_location(location);
    }
    else {
        response->object().set_location(controller_endpoint);
    }
    LOG_INFO(
        "Remote COP/MOVE complete ok from location: "
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

HsmObjectStoreResponse::Ptr
DistributedHsmObjectStoreClient::do_local_copy_or_move(
    const HsmObjectStoreRequest& request, bool is_copy) const
{
    Stream stream;

    HsmObjectStoreRequest get_request(
        request.object().id(), HsmObjectStoreRequestMethod::GET);
    get_request.set_source_tier(request.source_tier());
    get_request.set_extent(request.extent());

    HsmObjectStoreRequest put_request(
        request.object().id(), HsmObjectStoreRequestMethod::PUT);
    put_request.set_target_tier(request.target_tier());
    put_request.set_extent(request.extent());

    HsmObjectStoreResponse::Ptr get_response;
    if (auto hsm_client =
            m_client_manager->get_hsm_client(request.source_tier());
        hsm_client != nullptr) {
        get_response = hsm_client->make_request(get_request, &stream);
    }
    else {
        auto client = m_client_manager->get_client(request.source_tier());
        assert(client != nullptr);
        auto response = client->make_request(
            HsmObjectStoreRequest::to_base_request(get_request), &stream);
        get_response =
            HsmObjectStoreResponse::create(get_request, std::move(response));
    }
    if (!get_response->ok()) {
        return get_response;
    }

    HsmObjectStoreResponse::Ptr put_response;
    if (auto hsm_client =
            m_client_manager->get_hsm_client(request.target_tier());
        hsm_client != nullptr) {
        put_response = hsm_client->make_request(put_request, &stream);
    }
    else {
        auto client = m_client_manager->get_client(request.target_tier());
        assert(client != nullptr);
        auto response = client->make_request(
            HsmObjectStoreRequest::to_base_request(put_request), &stream);
        put_response =
            HsmObjectStoreResponse::create(put_request, std::move(response));
    }
    if (!put_response->ok()) {
        return put_response;
    }

    auto result = HsmObjectStoreResponse::create(request, m_id);

    auto stream_result = stream.flush();
    if (!stream_result.ok()) {
        result->on_error(
            {HsmObjectStoreErrorCode::ERROR,
             "Failed to flush stream copying or moving between clients"});
        return result;
    }

    if (!is_copy) {
        HsmObjectStoreRequest release_request(
            request.object().id(), HsmObjectStoreRequestMethod::REMOVE);
        release_request.set_source_tier(request.source_tier());
        release_request.set_extent(request.extent());

        HsmObjectStoreResponse::Ptr release_response;
        if (auto hsm_client =
                m_client_manager->get_hsm_client(request.source_tier());
            hsm_client != nullptr) {
            release_response = hsm_client->make_request(release_request);
        }
        else {
            auto client = m_client_manager->get_client(request.source_tier());
            assert(client != nullptr);
            auto response = client->make_request(
                HsmObjectStoreRequest::to_base_request(release_request));
            release_response = HsmObjectStoreResponse::create(
                put_request, std::move(response));
        }
        if (!release_response->ok()) {
            return release_response;
        }
    }
    return result;
}

HsmObjectStoreResponse::Ptr
DistributedHsmObjectStoreClient::do_remote_copy_or_move_with_local_source(
    const HsmObjectStoreRequest& request, bool is_copy) const
{
    auto response = HsmObjectStoreResponse::create(request, m_id);

    if (m_http_client == nullptr) {
        const std::string message =
            "Object store client needs to hit remote but has no http client - possible config issue.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    auto controller_endpoint =
        m_hsm_service->get_self_config().m_controller_address;

    Stream stream;
    HsmObjectStoreResponse::Ptr get_response;
    HsmObjectStoreRequest get_request(
        request.object().id(), HsmObjectStoreRequestMethod::GET);
    get_request.set_source_tier(request.source_tier());
    get_request.set_extent(request.extent());

    if (auto hsm_client =
            m_client_manager->get_hsm_client(request.source_tier());
        hsm_client != nullptr) {
        get_response = hsm_client->make_request(get_request, &stream);
    }
    else {
        auto client = m_client_manager->get_client(request.source_tier());
        assert(client != nullptr);
        auto response = client->make_request(
            HsmObjectStoreRequest::to_base_request(get_request), &stream);
        get_response =
            HsmObjectStoreResponse::create(get_request, std::move(response));
    }
    if (!get_response->ok()) {
        return get_response;
    }

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    action.set_subject_key(request.object().get_primary_key());
    action.set_target_tier(request.target_tier());
    action.set_size(stream.get_source_size());
    action.set_primary_key(request.get_action_id());

    const auto path = controller_endpoint + "/api/v1/"
                      + hestia::HsmItem::hsm_action_name + "s";

    HttpRequest http_request(path, HttpRequest::Method::PUT);
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    http_request.get_header().set_items(flat_dict);
    http_request.get_header().set_item(
        "Authorisation",
        request.object().metadata().get_item("hestia-user_token"));

    const auto http_response =
        m_http_client->make_request(http_request, &stream);
    if (http_response->error()) {
        LOG_ERROR("Failed in remote put request: " << http_response->message());
        response->on_error(
            {HsmObjectStoreErrorCode::ERROR, http_response->message()});
        return response;
    }
    LOG_INFO("Remote COPY/MOVE complete ok");

    if (!is_copy) {
        HsmObjectStoreRequest release_request(
            request.object().id(), HsmObjectStoreRequestMethod::REMOVE);
        release_request.set_source_tier(request.source_tier());
        release_request.set_extent(request.extent());

        HsmObjectStoreResponse::Ptr release_response;
        if (auto hsm_client =
                m_client_manager->get_hsm_client(request.source_tier());
            hsm_client != nullptr) {
            release_response = hsm_client->make_request(release_request);
        }
        else {
            auto client = m_client_manager->get_client(request.source_tier());
            assert(client != nullptr);
            auto response = client->make_request(
                HsmObjectStoreRequest::to_base_request(release_request));
            release_response = HsmObjectStoreResponse::create(
                get_request, std::move(response));
        }
        if (!release_response->ok()) {
            return release_response;
        }
    }
    return response;
}

HsmObjectStoreResponse::Ptr
DistributedHsmObjectStoreClient::do_remote_copy_or_move_with_local_target(
    const HsmObjectStoreRequest& request, bool is_copy) const
{
    auto response = HsmObjectStoreResponse::create(request, m_id);

    if (m_http_client == nullptr) {
        const std::string message =
            "Object store client needs to hit remote but has no http client - possible config issue.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    auto controller_endpoint =
        m_hsm_service->get_self_config().m_controller_address;

    Stream stream;

    HsmObjectStoreResponse::Ptr put_response;
    HsmObjectStoreRequest put_request(
        request.object().id(), HsmObjectStoreRequestMethod::PUT);
    put_request.set_target_tier(request.target_tier());
    put_request.set_extent(request.extent());

    if (auto hsm_client =
            m_client_manager->get_hsm_client(request.source_tier());
        hsm_client != nullptr) {
        put_response = hsm_client->make_request(put_request, &stream);
    }
    else {
        auto client = m_client_manager->get_client(request.target_tier());
        assert(client != nullptr);
        auto response = client->make_request(
            HsmObjectStoreRequest::to_base_request(put_request), &stream);
        put_response =
            HsmObjectStoreResponse::create(put_request, std::move(response));
    }
    if (!put_response->ok()) {
        return put_response;
    }

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    action.set_subject_key(request.object().get_primary_key());
    action.set_source_tier(request.source_tier());
    action.set_primary_key(request.get_action_id());

    const auto path = controller_endpoint + "/api/v1/"
                      + hestia::HsmItem::hsm_action_name + "s";

    HttpRequest http_request(path, HttpRequest::Method::PUT);
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    http_request.get_header().set_items(flat_dict);
    http_request.get_header().set_item(
        "Authorisation",
        request.object().metadata().get_item("hestia-user_token"));

    const auto http_response =
        m_http_client->make_request(http_request, &stream);
    if (http_response->error()) {
        LOG_ERROR("Failed in remote get request: " << http_response->message());
        response->on_error(
            {HsmObjectStoreErrorCode::ERROR, http_response->message()});
        return response;
    }

    if (!is_copy) {
        return do_remote_release(request);
    }
    LOG_INFO("Remote COPY/MOVE complete ok");
    return response;
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::do_remote_release(
    const HsmObjectStoreRequest& request) const
{
    auto response = HsmObjectStoreResponse::create(request, m_id);

    if (m_http_client == nullptr) {
        const std::string message =
            "Object store client needs to hit remote but has no http client - possible config issue.";
        LOG_ERROR(message);
        response->on_error({HsmObjectStoreErrorCode::ERROR, message});
        return response;
    }

    auto controller_endpoint =
        m_hsm_service->get_self_config().m_controller_address;

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::RELEASE_DATA);
    action.set_subject_key(request.object().get_primary_key());
    action.set_source_tier(request.source_tier());
    action.set_size(request.extent().m_length);
    action.set_offset(request.extent().m_offset);
    action.set_primary_key(request.get_action_id());

    const auto path = controller_endpoint + "/api/v1/"
                      + hestia::HsmItem::hsm_action_name + "s";

    HttpRequest http_request(path, HttpRequest::Method::PUT);
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    http_request.get_header().set_items(flat_dict);
    http_request.get_header().set_item(
        "Authorisation",
        request.object().metadata().get_item("hestia-user_token"));

    const auto http_response = m_http_client->make_request(http_request);
    if (http_response->error()) {
        LOG_ERROR("Failed in remote get request: " << http_response->message());
        response->on_error(
            {HsmObjectStoreErrorCode::ERROR, http_response->message()});
        return response;
    }

    if (auto location = http_response->header().get_item("location");
        !location.empty()) {
        response->object().set_location(location);
    }
    else {
        response->object().set_location(controller_endpoint);
    }
    LOG_INFO(
        "Remote remove complete ok from location: "
        << response->object().get_location());
    return response;
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request, Stream* stream) const noexcept
{
    const auto current_user_id =
        m_hsm_service->get_user_service()->get_current_user().get_primary_key();

    if (request.method() == HsmObjectStoreRequestMethod::GET
        || request.method() == HsmObjectStoreRequestMethod::EXISTS
        || request.method() == HsmObjectStoreRequestMethod::REMOVE) {
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
        else if (request.method() == HsmObjectStoreRequestMethod::REMOVE) {
            LOG_INFO(
                "Did not find client for source tier "
                + std::to_string(request.source_tier())
                + "- creating remote release");
            return do_remote_release(request);
        }
    }
    else if (request.method() == HsmObjectStoreRequestMethod::PUT) {
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
                return do_local_copy_or_move(
                    request,
                    request.method() == HsmObjectStoreRequestMethod::COPY);
            }
        }
        else if (m_client_manager->has_client(request.source_tier())) {
            LOG_INFO("Only have source client for COPY/MOVE");
            return do_remote_copy_or_move_with_local_source(
                request, request.method() == HsmObjectStoreRequestMethod::COPY);
        }
        else if (m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Only have target client for COPY/MOVE");
            return do_remote_copy_or_move_with_local_target(
                request, request.method() == HsmObjectStoreRequestMethod::COPY);
        }
        else {
            LOG_INFO("Don't have local clients for COPY/MOVE - try remote");
            return do_remote_copy_or_move(
                request, request.method() == HsmObjectStoreRequestMethod::COPY);
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
