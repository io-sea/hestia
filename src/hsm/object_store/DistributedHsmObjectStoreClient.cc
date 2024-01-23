#include "DistributedHsmObjectStoreClient.h"

#include "ObjectStoreClientFactory.h"
#include "ObjectStoreClientManager.h"

#include "DistributedHsmService.h"
#include "HsmActionRequestAdapter.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "HsmService.h"
#include "Logger.h"
#include "StorageTier.h"

#include "ErrorUtils.h"

#include "HttpClient.h"
#include "S3Client.h"
#include <cassert>

namespace hestia {
DistributedHsmObjectStoreClient::DistributedHsmObjectStoreClient(
    ObjectStoreClientManager::Ptr client_manager,
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
    auto client_factory =
        std::make_unique<ObjectStoreClientFactory>(std::move(plugin_handler));
    auto client_manager =
        std::make_unique<ObjectStoreClientManager>(std::move(client_factory));
    return std::make_unique<DistributedHsmObjectStoreClient>(
        std::move(client_manager), http_client, s3_client);
}

OpStatus DistributedHsmObjectStoreClient::do_initialize(
    const std::string& cache_path, DistributedHsmService* hsm_service)
{
    m_hsm_service = hsm_service;

    ObjectStoreClientsConfig clients_config;
    clients_config.m_cache_path = cache_path;
    clients_config.m_s3_client  = m_s3_client;
    clients_config.m_node_id =
        m_hsm_service->get_self_config().m_self.get_primary_key();
    const auto op_status = read_tiers(clients_config.m_tiers);
    if (!op_status.ok()) {
        return op_status;
    }
    m_client_manager->setup_clients(clients_config);
    return {};
}

OpStatus DistributedHsmObjectStoreClient::read_tiers(
    std::vector<StorageTier>& tiers) const
{
    const auto user_ctx =
        m_hsm_service->get_user_service()->get_current_user_context();

    auto response = m_hsm_service->get_hsm_service()
                        ->get_service(HsmItem::Type::TIER)
                        ->make_request(CrudRequest{
                            CrudMethod::READ,
                            CrudQuery{CrudQuery::BodyFormat::ITEM}, user_ctx});
    if (!response->ok()) {
        const std::string msg =
            "Error reading tiers:" + response->get_error().to_string();
        LOG_ERROR(msg);
        return OpStatus(OpStatus::Status::ERROR, 0, SOURCE_LOC() + " | " + msg);
    }
    for (const auto& item : response->items()) {
        const auto tier = dynamic_cast<const StorageTier*>(item.get());
        tiers.emplace_back(*tier);
    }
    return {};
}

void DistributedHsmObjectStoreClient::on_error(
    const HsmObjectStoreContext& ctx,
    HsmObjectStoreErrorCode error_code,
    const std::string& msg) const
{
    LOG_ERROR(msg);
    auto response = HsmObjectStoreResponse::create(ctx.m_request, m_id);
    response->on_error({error_code, SOURCE_LOC() + " | " + msg});
    ctx.finish(std::move(response));
}

std::string DistributedHsmObjectStoreClient::get_action_path() const
{
    LOG_INFO("CTR address is: " + m_hsm_service->get_controller_address());
    return m_hsm_service->get_controller_address()
           + HsmActionRequestAdapter::get_endpoint();
}

void DistributedHsmObjectStoreClient::do_remote_op(
    const HttpRequest http_request, HsmObjectStoreContext& ctx) const
{
    auto http_completion_func = [this, ctx](HttpResponse::Ptr http_response) {
        auto response = HsmObjectStoreResponse::create(ctx.m_request, m_id);
        response->set_is_handled_remote(true);

        if (http_response->error()) {
            on_error(
                ctx, HsmObjectStoreErrorCode::ERROR,
                "Http client error: " + http_response->get_error_message());
            return;
        }

        if (auto location = http_response->header().get_item("location");
            !location.empty()) {
            response->object().set_location(location);
        }
        else {
            response->object().set_location(
                m_hsm_service->get_controller_address());
        }
        LOG_INFO(
            "Remote complete ok from location: "
            << response->object().get_location());
        ctx.finish(std::move(response));
    };

    auto http_progress_func = [this, ctx](std::size_t bytes_transferred) {
        auto response = HsmObjectStoreResponse::create(ctx.m_request, m_id);
        response->set_bytes_transferred(bytes_transferred);
        ctx.m_progress_func(std::move(response));
    };

    m_http_client->make_request(
        http_request, http_completion_func, ctx.m_stream,
        ctx.m_request.get_progress_interval(), http_progress_func);
}

bool DistributedHsmObjectStoreClient::check_remote_config(
    HsmObjectStoreContext& ctx, bool can_be_worker) const
{
    if (m_http_client == nullptr) {
        on_error(
            ctx, HsmObjectStoreErrorCode::CONFIG_ERROR,
            "Object store client needs to hit remote but has no http client - possible config issue.");
        return false;
    }

    bool is_worker =
        m_hsm_service->is_server() && !m_hsm_service->is_controller();
    if (!can_be_worker && is_worker) {
        on_error(
            ctx, HsmObjectStoreErrorCode::CONFIG_ERROR,
            "Attempting remote op from a Server-Worker. Backends have been misconfigured.");
        return false;
    }
    return true;
}

void DistributedHsmObjectStoreClient::do_remote_op(
    HsmObjectStoreContext& ctx, const std::string& endpoint) const
{
    if (!check_remote_config(ctx, !endpoint.empty())) {
        return;
    }

    auto action_type = HsmActionRequestAdapter::get_action_type(ctx.m_request);
    HsmAction action(HsmItem::Type::OBJECT, action_type);
    HsmActionRequestAdapter::to_action(action, ctx.m_request);
    if (action_type == HsmAction::Action::GET_DATA) {
        action.set_size(ctx.m_request.object().size());
    }
    else if (action_type == HsmAction::Action::PUT_DATA) {
        action.set_size(ctx.m_stream->get_source_size());
    }
    action.set_preferred_node_address(endpoint);

    HttpRequest http_request(
        get_action_path(),
        HsmActionRequestAdapter::get_http_method(ctx.m_request));
    HsmActionRequestAdapter::to_request(
        http_request, action,
        HsmActionRequestAdapter::get_user_token(ctx.m_request));
    if (action_type == HsmAction::Action::PUT_DATA) {
        http_request.set_content_length(ctx.m_stream->get_source_size());
    }
    do_remote_op(http_request, ctx);
}

void DistributedHsmObjectStoreClient::do_local_op(
    HsmObjectStoreContext& ctx) const
{
    const auto tier_id = ctx.m_request.is_source_tier_request() ?
                             ctx.m_request.source_tier() :
                             ctx.m_request.target_tier();
    if (auto backend = m_client_manager->get_backend(tier_id);
        backend != nullptr) {

        auto redirect_wrap_func = [ctx](HsmObjectStoreResponse::Ptr response) {
            if (!response->object().get_location().empty()) {
                LOG_INFO(
                    "Object store issued redirect - will follow it to remote - "
                    + response->object().get_location());
            }
            LOG_INFO("Finished local op");
            ctx.finish(std::move(response));
        };
        ctx.override_completion_func(redirect_wrap_func);
        backend->make_request(ctx);
    }
    else {
        const std::string msg =
            "Failed to locate local backend in dist object store request";
        auto response = HsmObjectStoreResponse::create(ctx.m_request, m_id);
        response->on_error(
            {HsmObjectStoreErrorCode::CONFIG_ERROR,
             SOURCE_LOC() + " | " + msg});
        ctx.finish(std::move(response));
    }
}

void DistributedHsmObjectStoreClient::do_local_hsm(
    HsmObjectStoreContext& ctx) const
{
    Stream stream;

    HsmObjectStoreResponse::Ptr get_response;
    auto get_completion =
        [&get_response](HsmObjectStoreResponse::Ptr response) {
            LOG_INFO("Local get completed ok");
            get_response = std::move(response);
        };
    HsmObjectStoreContext get_ctx(
        HsmObjectStoreContext::replace_method(
            ctx.m_request, HsmObjectStoreRequestMethod::GET),
        get_completion, nullptr, &stream);
    do_local_op(get_ctx);
    if (get_response && !get_response->ok()) {
        LOG_INFO("Local hsm get did not complete ok");
        ctx.finish(std::move(get_response));
        return;
    }

    HsmObjectStoreResponse::Ptr put_response;
    auto put_completion =
        [&put_response](HsmObjectStoreResponse::Ptr response) {
            LOG_INFO("Local put completed ok");
            put_response = std::move(response);
        };
    HsmObjectStoreContext put_ctx(
        HsmObjectStoreContext::replace_method(
            ctx.m_request, HsmObjectStoreRequestMethod::PUT),
        put_completion, nullptr, &stream);
    do_local_op(put_ctx);
    if (put_response && !put_response->ok()) {
        LOG_INFO("Local hsm put did not complete ok");
        ctx.finish(std::move(put_response));
        return;
    }

    auto stream_completion_func = [this, ctx](StreamState stream_state) {
        LOG_INFO("Stream complete");
        auto result = HsmObjectStoreResponse::create(ctx.m_request, m_id);
        if (!stream_state.ok()) {
            result->on_error(
                {HsmObjectStoreErrorCode::ERROR, stream_state.message()});
            ctx.finish(std::move(result));
            return;
        }
        if (ctx.m_request.method() == HsmObjectStoreRequestMethod::COPY) {
            ctx.finish(std::move(result));
        }
        else {
            auto remove_ctx =
                ctx.replace_method(HsmObjectStoreRequestMethod::REMOVE);
            do_local_op(remove_ctx);
        }
    };
    stream.set_completion_func(stream_completion_func);

    LOG_INFO("Starting stream flush");
    stream.flush(m_config.m_buffer_size);
    LOG_INFO("Finished stream flush");
}

void DistributedHsmObjectStoreClient::do_remote_hsm_with_local_source(
    HsmObjectStoreContext& ctx) const
{
    check_remote_config(ctx);

    Stream stream;

    HsmObjectStoreResponse::Ptr get_response;
    auto get_completion =
        [&get_response](HsmObjectStoreResponse::Ptr response) {
            get_response = std::move(response);
        };
    HsmObjectStoreContext get_ctx(
        HsmObjectStoreContext::replace_method(
            ctx.m_request, HsmObjectStoreRequestMethod::GET),
        get_completion, nullptr, &stream);
    do_local_op(get_ctx);
    if (get_response && !get_response->ok()) {
        ctx.finish(std::move(get_response));
        return;
    }

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    HsmActionRequestAdapter::to_action(action, ctx.m_request);
    action.set_size(stream.get_source_size());

    HttpRequest http_request(get_action_path(), HttpRequest::Method::PUT);
    HsmActionRequestAdapter::to_request(
        http_request, action,
        HsmActionRequestAdapter::get_user_token(ctx.m_request));

    auto http_completion_cb = [this, ctx](HttpResponse::Ptr http_response) {
        auto local_ctx = ctx;
        on_remote_response(std::move(http_response), local_ctx);
    };
    m_http_client->make_request(http_request, http_completion_cb, &stream);
}

void DistributedHsmObjectStoreClient::do_remote_hsm_with_local_target(
    HsmObjectStoreContext& ctx) const
{
    check_remote_config(ctx);

    Stream stream;

    HsmObjectStoreResponse::Ptr put_response;
    auto put_completion =
        [&put_response](HsmObjectStoreResponse::Ptr response) {
            put_response = std::move(response);
        };
    HsmObjectStoreContext put_ctx(
        HsmObjectStoreContext::replace_method(
            ctx.m_request, HsmObjectStoreRequestMethod::PUT),
        put_completion, nullptr, &stream);

    do_local_op(put_ctx);
    if (put_response && !put_response->ok()) {
        ctx.finish(std::move(put_response));
        return;
    }

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    HsmActionRequestAdapter::to_action(action, ctx.m_request);

    HttpRequest http_request(get_action_path(), HttpRequest::Method::GET);
    HsmActionRequestAdapter::to_request(
        http_request, action,
        HsmActionRequestAdapter::get_user_token(ctx.m_request));

    auto http_completion_cb = [this, ctx](HttpResponse::Ptr http_response) {
        auto local_ctx = ctx;
        on_remote_response(std::move(http_response), local_ctx);
    };
    m_http_client->make_request(http_request, http_completion_cb, &stream);
}

void DistributedHsmObjectStoreClient::on_remote_response(
    HttpResponse::Ptr http_response, HsmObjectStoreContext& ctx) const
{
    if (http_response->error()) {
        on_error(
            ctx, HsmObjectStoreErrorCode::ERROR,
            "Http response response error: "
                + http_response->get_error_message());
        return;
    }
    if (ctx.m_request.method() == HsmObjectStoreRequestMethod::COPY) {
        on_success(ctx);
    }
    else {
        auto remove_ctx =
            ctx.replace_method(HsmObjectStoreRequestMethod::REMOVE);
        do_local_op(remove_ctx);
    }
}

void DistributedHsmObjectStoreClient::make_single_tier_request(
    HsmObjectStoreContext& ctx) const noexcept
{
    const auto tier_id = ctx.m_request.is_source_tier_request() ?
                             ctx.m_request.source_tier() :
                             ctx.m_request.target_tier();
    if (m_client_manager->has_backend(tier_id)) {
        LOG_INFO("Found local client for tier: " << tier_id);
        do_local_op(ctx);
    }
    else {
        LOG_INFO(
            "Did not find client for tier " + tier_id + " - checking remote");
        do_remote_op(ctx, ctx.m_request.get_preferred_address());
    }
}

void DistributedHsmObjectStoreClient::make_multi_tier_request(
    HsmObjectStoreContext& ctx) const noexcept
{
    if (!ctx.m_request.is_hsm_only_request()) {
        on_error(
            ctx, HsmObjectStoreErrorCode::ERROR,
            "Method not handled by dist object store client");
        return;
    }

    if (m_client_manager->has_backend(ctx.m_request.source_tier())
        && m_client_manager->has_backend(ctx.m_request.target_tier())) {
        LOG_INFO("Both clients for COPY/MOVE found");

        if (m_client_manager->have_same_backend(
                ctx.m_request.source_tier(), ctx.m_request.target_tier())) {
            LOG_INFO("Same clients handling COPY/MOVE");
            auto client =
                m_client_manager->get_backend(ctx.m_request.target_tier());
            client->make_request(ctx);
            return;
        }
        else {
            LOG_INFO("Doing COPY/MOVE between different clients");
            do_local_hsm(ctx);
            return;
        }
    }
    else if (m_client_manager->has_backend(ctx.m_request.source_tier())) {
        LOG_INFO("Only have source client for COPY/MOVE");
        do_remote_hsm_with_local_source(ctx);
        return;
    }
    else if (m_client_manager->has_backend(ctx.m_request.target_tier())) {
        LOG_INFO("Only have target client for COPY/MOVE");
        do_remote_hsm_with_local_target(ctx);
        return;
    }
    else {
        LOG_INFO("Don't have local clients for COPY/MOVE - try remote");
        do_remote_op(ctx);
        return;
    }
}

void DistributedHsmObjectStoreClient::make_request(
    HsmObjectStoreContext& ctx) const noexcept
{
    if (ctx.m_request.is_single_tier_request()) {
        make_single_tier_request(ctx);
    }
    else {
        make_multi_tier_request(ctx);
    }
}

bool DistributedHsmObjectStoreClient::is_controller_node() const
{
    return m_hsm_service->get_self_config().m_self.is_controller();
}

void DistributedHsmObjectStoreClient::get(HsmObjectStoreContext&) const {}

void DistributedHsmObjectStoreClient::put(HsmObjectStoreContext&) const {}

void DistributedHsmObjectStoreClient::copy(HsmObjectStoreContext&) const {}

void DistributedHsmObjectStoreClient::move(HsmObjectStoreContext&) const {}

void DistributedHsmObjectStoreClient::remove(HsmObjectStoreContext&) const {}

}  // namespace hestia
