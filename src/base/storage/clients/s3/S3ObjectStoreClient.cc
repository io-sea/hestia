#include "S3ObjectStoreClient.h"

#include "ProjectConfig.h"

#include "S3Bucket.h"
#include "S3Object.h"

#include "ErrorUtils.h"
#include "Logger.h"

#include <iostream>
#include <stdexcept>

namespace hestia {
S3ObjectStoreClient::S3ObjectStoreClient(S3Client* s3_client) :
    m_s3_client(s3_client)
{
    m_container_adapter = S3BucketAdapter::create("s3_client");
    m_object_adapter    = S3ObjectAdapter::create("s3_client");
}

S3ObjectStoreClient::Ptr S3ObjectStoreClient::create(S3Client* s3_client)
{
    return std::make_unique<S3ObjectStoreClient>(s3_client);
}

S3ObjectStoreClient::Ptr S3ObjectStoreClient::create(
    const std::string& id, const S3Config& config, S3Client* s3_client)
{
    auto instance = create(s3_client);
    instance->do_initialize(id, {}, config);
    return instance;
}

std::string S3ObjectStoreClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name() + "::S3Client";
}

void S3ObjectStoreClient::initialize(
    const std::string& id,
    const std::string& cache_path,
    const Dictionary& config_data)
{
    m_config.deserialize(config_data);
    do_initialize(id, cache_path, m_config);
}

void S3ObjectStoreClient::do_initialize(
    const std::string& id, const std::string&, const S3Config& config)
{
    m_config = config;
    m_id     = id;

    m_container_adapter =
        S3BucketAdapter::create(m_config.get_metadata_prefix());
    m_object_adapter = S3ObjectAdapter::create(m_config.get_metadata_prefix());
}

void S3ObjectStoreClient::init_s3_request(
    const ObjectStoreContext& ctx, S3Request& req) const
{
    const auto domain    = m_config.get_default_host();
    req.m_endpoint       = domain;
    req.m_signed_headers = {"host", "x-amz-content-sha256", "x-amz-date"};
    req.m_range          = S3Range(
        ctx.m_request.extent().m_offset, ctx.m_request.extent().m_length);
    req.set_extra_headers(m_config.get_extra_headers());
}

void S3ObjectStoreClient::create_bucket_if_needed(
    const S3Bucket& bucket, const S3Request& req) const
{
    const auto bucket_status = m_s3_client->head_bucket(bucket, req);
    if (bucket_status.is_ok()) {
        return;
    }

    if (bucket_status.get_s3_code() == S3StatusCode::_404_NO_SUCH_BUCKET
        || bucket_status.get_code_and_id().first == 404) {
        LOG_INFO("Bucket not found - creating one");
        const auto bucket_create_status =
            m_s3_client->create_bucket(bucket, req);
        if (!bucket_create_status.is_ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + " | Error creating s3 bucket: "
                + bucket_create_status.to_string());
        }
    }
    else {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Error checking s3 bucket: " + bucket_status.to_string());
    }
}

void S3ObjectStoreClient::put(ObjectStoreContext& ctx) const
{
    const auto domain = m_config.get_default_host();

    S3Request s3_request(domain);
    init_s3_request(ctx, s3_request);

    S3Object s3_object;
    m_object_adapter->to_s3(s3_object, s3_request, ctx.m_request.object());

    auto bucket_name = s3_object.m_bucket;
    if (bucket_name.empty()) {
        bucket_name = m_config.get_default_bucket_name();
    }

    S3Bucket s3_bucket(bucket_name);

    /*
    std::string user_id = obj.m_user;
    if (auto config_id = m_config.get_access_key_id(); !config_id.empty()) {
        user_id = config_id;
    }

    std::string user_secret = obj.m_user_token;
    if (auto config_secret = m_config.get_secret_access_key();
        !config_secret.empty()) {
        user_secret = config_secret;
    }
    */

    create_bucket_if_needed(s3_bucket, s3_request);

    LOG_DEBUG(
        "Doing PUT with StorageObject: " + ctx.m_request.object().to_string());
    LOG_DEBUG("Doing PUT with S3Object: " + s3_object.to_string());

    auto s3_completion_func = [this, ctx](S3Response::Ptr response) {
        if (!response->m_status.is_ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + " | Error putting s3 object: "
                + response->m_status.to_string());
        }
        on_success(ctx);
    };

    auto stream_progress_func = [ctx, id = m_id](std::size_t transferred) {
        auto response = ObjectStoreResponse::create(ctx.m_request, id);
        response->set_bytes_transferred(transferred);
        ctx.m_progress_func(std::move(response));
    };

    m_s3_client->put_object(
        s3_object, s3_bucket, s3_request, ctx.m_stream, s3_completion_func,
        ctx.m_request.get_progress_interval(), stream_progress_func);
}

void S3ObjectStoreClient::get(ObjectStoreContext& ctx) const
{
    const auto domain = m_config.get_default_host();

    S3Request s3_request(domain);
    init_s3_request(ctx, s3_request);

    S3Object s3_object;
    m_object_adapter->to_s3(s3_object, s3_request, ctx.m_request.object());

    auto bucket_name = s3_object.m_bucket;
    if (bucket_name.empty()) {
        bucket_name = m_config.get_default_bucket_name();
    }
    S3Bucket s3_bucket(bucket_name);

    LOG_DEBUG(
        "Doing GET with StorageObject: " + ctx.m_request.object().to_string());
    LOG_DEBUG("Doing GET with S3Object: " + s3_object.to_string());

    auto s3_completion_func = [this, ctx,
                               s3_bucket](S3Response::Ptr s3_response) {
        if (!s3_response->m_status.is_ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + " | Error getting s3 object: "
                + s3_response->m_status.to_string());
        }
        auto response = ObjectStoreResponse::create(ctx.m_request, m_id);
        m_object_adapter->from_s3(
            response->object(), s3_bucket, s3_response->m_content);
        ctx.finish(std::move(response));
    };

    auto stream_progress_func = [ctx, id = m_id](std::size_t transferred) {
        auto response = ObjectStoreResponse::create(ctx.m_request, id);
        response->set_bytes_transferred(transferred);
        ctx.m_progress_func(std::move(response));
    };

    m_s3_client->get_object(
        s3_object, s3_bucket, s3_request, ctx.m_stream, s3_completion_func,
        ctx.m_request.get_progress_interval(), stream_progress_func);
}

void S3ObjectStoreClient::remove(const StorageObject& object) const
{
    const auto domain = m_config.get_default_host();

    S3Request s3_request(domain);
    s3_request.m_endpoint       = domain;
    s3_request.m_signed_headers = {
        "host", "x-amz-content-sha256", "x-amz-date"};

    S3Object s3_object;
    m_object_adapter->to_s3(s3_object, s3_request, object);

    S3Bucket s3_bucket(s3_object.m_bucket);

    const auto status =
        m_s3_client->delete_object(s3_object, s3_bucket, s3_request);
    if (!status.is_ok()) {
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Error deleting s3 object: " + status.to_string());
    }
}

void S3ObjectStoreClient::list(
    const KeyValuePair& query, std::vector<StorageObject>& found) const
{
    (void)query;
    (void)found;
}

bool S3ObjectStoreClient::exists(const StorageObject& object) const
{
    (void)object;
    return false;
}
}  // namespace hestia