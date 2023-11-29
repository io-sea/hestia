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

OpStatus DistributedHsmObjectStoreClient::do_initialize(
    const std::string& cache_path, DistributedHsmService* hsm_service)
{
    m_hsm_service = hsm_service;

    const auto current_user_ctx =
        m_hsm_service->get_user_service()->get_current_user_context();

    auto response =
        m_hsm_service->get_hsm_service()
            ->get_service(HsmItem::Type::TIER)
            ->make_request(CrudRequest{
                CrudMethod::READ, CrudQuery{CrudQuery::BodyFormat::ITEM},
                current_user_ctx});
    if (!response->ok()) {
        LOG_ERROR("Error reading tiers:" << response->get_error().to_string());
        return OpStatus(
            OpStatus::Status::ERROR, 0, response->get_error().to_string());
    }

    std::vector<StorageTier> tiers;
    for (const auto& item : response->items()) {
        const auto tier = dynamic_cast<const StorageTier*>(item.get());
        LOG_INFO(
            "Adding tier with priority: "
            << std::to_string(tier->get_priority())
            << " with id: " << tier->get_primary_key());
        tiers.emplace_back(*tier);
    }

    m_client_manager->setup_clients(
        cache_path, m_hsm_service->get_self_config().m_self.get_primary_key(),
        m_s3_client, tiers, m_hsm_service->get_backends());
    return {};
}

void on_error(
    HsmObjectStoreResponse::Ptr response,
    HsmObjectStoreClient::completionFunc completion_func,
    HsmObjectStoreErrorCode code,
    const std::string& msg)
{
    const std::string full_msg = SOURCE_LOC() + " | " + msg;
    LOG_ERROR(msg);
    response->on_error({code, full_msg});
    completion_func(std::move(response));
}

void init_action(HsmAction& action, const HsmObjectStoreRequest& request)
{
    action.set_subject_key(request.object().get_primary_key());
    action.set_source_tier_id(request.source_tier());
    action.set_target_tier_id(request.target_tier());
    action.set_primary_key(request.get_action_id());
    action.set_size(request.extent().m_length);
    action.set_offset(request.extent().m_offset);
}

std::string get_action_path(const std::string& endpoint)
{
    return endpoint + "/api/v1/" + hestia::HsmItem::hsm_action_name + "s";
}

void init_request(
    HttpRequest& req, const HsmAction& action, const std::string& user_token)
{
    Dictionary dict;
    action.serialize(dict);

    Map flat_dict;
    dict.flatten(flat_dict);
    flat_dict.add_key_prefix("hestia.hsm_action.");

    req.get_header().set_items(flat_dict);
    if (!user_token.empty()) {
        req.get_header().set_item("Authorization", user_token);
    }
}

void init_request(
    HsmObjectStoreRequest& child_req, const HsmObjectStoreRequest& request)
{
    child_req.set_source_tier(request.source_tier());
    child_req.set_target_tier(request.target_tier());
    child_req.set_extent(request.extent());
}

void DistributedHsmObjectStoreClient::do_remote_op(
    const HttpRequest http_request,
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    const std::string& endpoint,
    Stream* stream) const
{
    auto http_completion_func = [completion_func, request, id = m_id,
                                 endpoint](HttpResponse::Ptr http_response) {
        auto response = HsmObjectStoreResponse::create(request, id);
        response->set_is_handled_remote(true);

        if (http_response->error()) {
            on_error(
                std::move(response), completion_func,
                HsmObjectStoreErrorCode::ERROR, http_response->message());
            return;
        }

        if (auto location = http_response->header().get_item("location");
            !location.empty()) {
            response->object().set_location(location);
        }
        else {
            response->object().set_location(endpoint);
        }
        LOG_INFO(
            "Remote complete ok from location: "
            << response->object().get_location());
        completion_func(std::move(response));
    };

    if (progress_func != nullptr) {
        auto http_progress_func = [progress_func, request,
                                   id = m_id](std::size_t bytes_transferred) {
            auto response = HsmObjectStoreResponse::create(request, id);
            response->set_bytes_transferred(bytes_transferred);
            progress_func(std::move(response));
        };
        m_http_client->make_request(
            http_request, http_completion_func, stream,
            request.get_progress_interval(), http_progress_func);
    }
    else if (stream != nullptr) {
        LOG_INFO("Makine req with stream");
        m_http_client->make_request(http_request, http_completion_func, stream);
    }
    else {
        m_http_client->make_request(http_request, http_completion_func);
    }
}

bool DistributedHsmObjectStoreClient::check_remote_config(
    const HsmObjectStoreRequest& request, completionFunc completion_func) const
{
    if (m_http_client == nullptr) {
        on_error(
            HsmObjectStoreResponse::create(request, m_id), completion_func,
            HsmObjectStoreErrorCode::CONFIG_ERROR,
            "Object store client needs to hit remote but has no http client - possible config issue.");
        return false;
    }

    if (m_hsm_service->is_server()) {
        on_error(
            HsmObjectStoreResponse::create(request, m_id), completion_func,
            HsmObjectStoreErrorCode::CONFIG_ERROR,
            "Attempting remote remote op from a Server-Worker. Backends have been misconfigured.");
        return false;
    }
    return true;
}

void DistributedHsmObjectStoreClient::do_remote_get(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    Stream* stream) const
{
    if (!check_remote_config(request, completion_func)) {
        return;
    }
    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    init_action(action, request);
    action.set_size(request.object().size());

    auto controller_endpoint = m_hsm_service->get_controller_address();
    const auto user_token =
        request.object().metadata().get_item("hestia-user_token");
    HttpRequest http_request(
        get_action_path(controller_endpoint), HttpRequest::Method::GET);
    init_request(http_request, action, user_token);

    LOG_INFO("Remote get - has stream ? " << bool(stream));

    do_remote_op(
        http_request, request, completion_func, progress_func,
        controller_endpoint, stream);
}

void DistributedHsmObjectStoreClient::do_remote_put(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    Stream* stream) const
{
    if (!check_remote_config(request, completion_func)) {
        return;
    }
    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    init_action(action, request);
    action.set_size(stream->get_source_size());

    auto controller_endpoint = m_hsm_service->get_controller_address();
    const auto user_token =
        request.object().metadata().get_item("hestia-user_token");
    HttpRequest http_request(
        get_action_path(controller_endpoint), HttpRequest::Method::PUT);
    init_request(http_request, action, user_token);
    http_request.set_content_length(stream->get_source_size());

    do_remote_op(
        http_request, request, completion_func, progress_func,
        controller_endpoint, stream);
}

void DistributedHsmObjectStoreClient::do_remote_copy_or_move(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    bool is_copy) const
{
    if (!check_remote_config(request, completion_func)) {
        return;
    }

    auto action_type =
        is_copy ? HsmAction::Action::COPY_DATA : HsmAction::Action::MOVE_DATA;

    HsmAction action(HsmItem::Type::OBJECT, action_type);
    init_action(action, request);

    auto controller_endpoint = m_hsm_service->get_controller_address();
    const auto user_token =
        request.object().metadata().get_item("hestia-user_token");
    HttpRequest http_request(
        get_action_path(controller_endpoint), HttpRequest::Method::PUT);
    init_request(http_request, action, user_token);

    do_remote_op(
        http_request, request, completion_func, progress_func,
        controller_endpoint);
}

void DistributedHsmObjectStoreClient::do_local_release(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func) const
{
    HsmObjectStoreRequest release_request(
        request.object().id(), HsmObjectStoreRequestMethod::REMOVE);
    init_request(release_request, request);

    if (auto hsm_client =
            m_client_manager->get_hsm_client(request.source_tier());
        hsm_client != nullptr) {
        hsm_client->make_request(
            release_request, completion_func, progress_func);
    }
    else {
        auto client = m_client_manager->get_client(request.source_tier());
        auto completion_cb = [release_request, completion_func](
                                 ObjectStoreResponse::Ptr response) {
            completion_func(HsmObjectStoreResponse::create(
                release_request, std::move(response)));
        };
        client->make_request(
            HsmObjectStoreRequest::to_base_request(release_request),
            completion_cb);
    }
}

void DistributedHsmObjectStoreClient::do_remote_release(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func) const
{
    if (!check_remote_config(request, completion_func)) {
        return;
    }

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::RELEASE_DATA);
    init_action(action, request);

    auto controller_endpoint = m_hsm_service->get_controller_address();
    const auto user_token =
        request.object().metadata().get_item("hestia-user_token");
    HttpRequest http_request(
        get_action_path(controller_endpoint), HttpRequest::Method::PUT);
    init_request(http_request, action, user_token);

    do_remote_op(
        http_request, request, completion_func, progress_func,
        controller_endpoint);
}

void DistributedHsmObjectStoreClient::do_local_op(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    Stream* stream,
    const std::string& tier) const
{
    if (auto hsm_client = m_client_manager->get_hsm_client(tier);
        hsm_client != nullptr) {
        hsm_client->make_request(
            request, completion_func, progress_func, stream);
    }
    else {
        auto client = m_client_manager->get_client(tier);
        auto object_store_completion_func =
            [request, completion_func](ObjectStoreResponse::Ptr response) {
                completion_func(HsmObjectStoreResponse::create(
                    request, std::move(response)));
            };
        auto object_store_progress_func =
            [request, progress_func](ObjectStoreResponse::Ptr response) {
                progress_func(HsmObjectStoreResponse::create(
                    request, std::move(response)));
            };
        client->make_request(
            HsmObjectStoreRequest::to_base_request(request),
            object_store_completion_func, object_store_progress_func, stream);
    }
}

HsmObjectStoreResponse::Ptr DistributedHsmObjectStoreClient::do_local_op(
    const HsmObjectStoreRequest& request,
    Stream* stream,
    const std::string& tier) const
{
    HsmObjectStoreResponse::Ptr outer_response;
    if (auto hsm_client = m_client_manager->get_hsm_client(tier);
        hsm_client != nullptr) {
        auto completion_cb =
            [&outer_response](HsmObjectStoreResponse::Ptr response) {
                outer_response = std::move(response);
            };
        hsm_client->make_request(request, completion_cb, nullptr, stream);
    }
    else {
        auto client = m_client_manager->get_client(tier);
        assert(client != nullptr);
        auto completion_cb = [request, &outer_response](
                                 ObjectStoreResponse::Ptr response) {
            outer_response =
                HsmObjectStoreResponse::create(request, std::move(response));
        };
        client->make_request(
            HsmObjectStoreRequest::to_base_request(request), completion_cb,
            nullptr, stream);
    }
    return outer_response;
}

void DistributedHsmObjectStoreClient::do_local_copy_or_move(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    bool is_copy) const
{
    Stream stream;

    HsmObjectStoreRequest get_request(
        request.object().id(), HsmObjectStoreRequestMethod::GET);
    init_request(get_request, request);

    HsmObjectStoreResponse::Ptr get_response =
        do_local_op(get_request, &stream, request.source_tier());
    if (get_response && !get_response->ok()) {
        completion_func(std::move(get_response));
    }

    HsmObjectStoreRequest put_request(
        request.object().id(), HsmObjectStoreRequestMethod::PUT);
    init_request(put_request, request);

    HsmObjectStoreResponse::Ptr put_response =
        do_local_op(put_request, &stream, request.target_tier());
    if (put_response && !put_response->ok()) {
        completion_func(std::move(put_response));
    }

    auto stream_completion_func = [this, request, completion_func,
                                   progress_func,
                                   is_copy](StreamState stream_state) {
        auto result = HsmObjectStoreResponse::create(request, "");
        if (!stream_state.ok()) {
            result->on_error(
                {HsmObjectStoreErrorCode::ERROR, stream_state.message()});
            completion_func(std::move(result));
            return;
        }
        if (is_copy) {
            completion_func(std::move(result));
        }
        else {
            do_local_release(request, completion_func, progress_func);
        }
    };
    stream.set_completion_func(stream_completion_func);
    stream.flush();
}

void DistributedHsmObjectStoreClient::do_remote_copy_or_move_with_local_source(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    bool is_copy) const
{
    check_remote_config(request, completion_func);

    Stream stream;
    HsmObjectStoreRequest get_request(
        request.object().id(), HsmObjectStoreRequestMethod::GET);
    init_request(get_request, request);

    HsmObjectStoreResponse::Ptr get_response =
        do_local_op(get_request, &stream, request.source_tier());
    if (get_response && !get_response->ok()) {
        completion_func(std::move(get_response));
    }

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    init_action(action, request);
    action.set_size(stream.get_source_size());

    const auto controller_endpoint = m_hsm_service->get_controller_address();
    const auto user_token =
        request.object().metadata().get_item("hestia-user_token");
    HttpRequest http_request(
        get_action_path(controller_endpoint), HttpRequest::Method::PUT);
    init_request(http_request, action, user_token);

    auto http_completion_cb = [this, request, completion_func, progress_func,
                               is_copy](HttpResponse::Ptr http_response) {
        auto response = HsmObjectStoreResponse::create(request, "");
        if (http_response->error()) {
            on_error(
                std::move(response), completion_func,
                HsmObjectStoreErrorCode::ERROR, http_response->message());
            return;
        }

        if (is_copy) {
            completion_func(std::move(response));
        }
        else {
            do_local_release(request, completion_func, progress_func);
        }
    };
    m_http_client->make_request(http_request, http_completion_cb, &stream);
}

void DistributedHsmObjectStoreClient::do_remote_copy_or_move_with_local_target(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    bool is_copy) const
{
    check_remote_config(request, completion_func);

    Stream stream;

    HsmObjectStoreRequest put_request(
        request.object().id(), HsmObjectStoreRequestMethod::PUT);
    init_request(put_request, request);

    HsmObjectStoreResponse::Ptr put_response =
        do_local_op(put_request, &stream, request.target_tier());
    if (put_response && !put_response->ok()) {
        completion_func(std::move(put_response));
    }

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    init_action(action, request);

    const auto controller_endpoint = m_hsm_service->get_controller_address();
    const auto user_token =
        request.object().metadata().get_item("hestia-user_token");
    HttpRequest http_request(
        get_action_path(controller_endpoint), HttpRequest::Method::GET);
    init_request(http_request, action, user_token);

    auto http_completion_cb = [this, request, completion_func, progress_func,
                               is_copy](HttpResponse::Ptr http_response) {
        auto response = HsmObjectStoreResponse::create(request, "");
        if (http_response->error()) {
            on_error(
                std::move(response), completion_func,
                HsmObjectStoreErrorCode::ERROR, http_response->message());
            return;
        }
        if (is_copy) {
            completion_func(std::move(response));
        }
        else {
            do_remote_release(request, completion_func, progress_func);
        }
    };
    m_http_client->make_request(http_request, http_completion_cb, &stream);
}

void DistributedHsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    Stream* stream) const noexcept
{
    if (request.method() == HsmObjectStoreRequestMethod::GET
        || request.method() == HsmObjectStoreRequestMethod::EXISTS
        || request.method() == HsmObjectStoreRequestMethod::REMOVE) {
        if (m_client_manager->has_client(request.source_tier())) {
            LOG_INFO(
                "Found local client for source tier: "
                << request.source_tier());
            do_local_op(
                request, completion_func, progress_func, stream,
                request.source_tier());
            return;
        }
        else if (request.method() == HsmObjectStoreRequestMethod::GET) {
            LOG_INFO(
                "Did not find client for source tier " + request.source_tier()
                + "- creating remote get");
            do_remote_get(request, completion_func, progress_func, stream);
            return;
        }
        else if (request.method() == HsmObjectStoreRequestMethod::REMOVE) {
            LOG_INFO(
                "Did not find client for source tier " + request.source_tier()
                + "- creating remote release");
            do_remote_release(request, completion_func, progress_func);
            return;
        }
    }
    else if (request.method() == HsmObjectStoreRequestMethod::PUT) {
        if (m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Found local client for target tier");
            do_local_op(
                request, completion_func, progress_func, stream,
                request.target_tier());
            return;
        }
        else if (request.method() == HsmObjectStoreRequestMethod::PUT) {
            LOG_INFO(
                "Did not find client for target tier " + request.target_tier()
                + " - creating remote put");
            do_remote_put(request, completion_func, progress_func, stream);
            return;
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
                client->make_request(
                    request, completion_func, progress_func, stream);
                return;
            }
            else {
                LOG_INFO("Doing COPY/MOVE between different clients");
                do_local_copy_or_move(
                    request, completion_func, progress_func,
                    request.method() == HsmObjectStoreRequestMethod::COPY);
                return;
            }
        }
        else if (m_client_manager->has_client(request.source_tier())) {
            LOG_INFO("Only have source client for COPY/MOVE");
            do_remote_copy_or_move_with_local_source(
                request, completion_func, progress_func,
                request.method() == HsmObjectStoreRequestMethod::COPY);
            return;
        }
        else if (m_client_manager->has_client(request.target_tier())) {
            LOG_INFO("Only have target client for COPY/MOVE");
            do_remote_copy_or_move_with_local_target(
                request, completion_func, progress_func,
                request.method() == HsmObjectStoreRequestMethod::COPY);
            return;
        }
        else {
            LOG_INFO("Don't have local clients for COPY/MOVE - try remote");
            do_remote_copy_or_move(
                request, completion_func, progress_func,
                request.method() == HsmObjectStoreRequestMethod::COPY);
            return;
        }
    }

    auto response = HsmObjectStoreResponse::create(request, "");
    on_error(
        std::move(response), completion_func, HsmObjectStoreErrorCode::ERROR,
        "Method not handled by dist object store client");
}

bool DistributedHsmObjectStoreClient::is_controller_node() const
{
    return m_hsm_service->get_self_config().m_self.is_controller();
}

void DistributedHsmObjectStoreClient::get(
    const HsmObjectStoreRequest&, Stream*, completionFunc, progressFunc) const
{
}

void DistributedHsmObjectStoreClient::put(
    const HsmObjectStoreRequest&, Stream*, completionFunc, progressFunc) const
{
}

void DistributedHsmObjectStoreClient::copy(
    const HsmObjectStoreRequest&, completionFunc, progressFunc) const
{
}

void DistributedHsmObjectStoreClient::move(
    const HsmObjectStoreRequest&, completionFunc, progressFunc) const
{
}

void DistributedHsmObjectStoreClient::remove(
    const HsmObjectStoreRequest&, completionFunc, progressFunc) const
{
}

}  // namespace hestia
