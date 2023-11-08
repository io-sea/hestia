#include "S3ObjectStoreClient.h"

#include "ProjectConfig.h"

#include "S3Bucket.h"
#include "S3Object.h"

#include "ErrorUtils.h"
#include "Logger.h"

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

void S3ObjectStoreClient::put(
    const ObjectStoreRequest& request,
    completionFunc completion_func,
    Stream* stream,
    Stream::progressFunc progress_func) const
{
    S3Request s3_request;
    s3_request.m_range =
        S3Range(request.extent().m_offset, request.extent().m_length);

    S3Object s3_object;
    m_object_adapter->to_s3(s3_object, s3_request, request.object());

    S3Bucket s3_bucket(s3_object.m_bucket);

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

    const auto bucket_status = m_s3_client->head_bucket(s3_bucket, s3_request);
    if (!bucket_status.is_ok()) {
        if (bucket_status.get_s3_code() == S3StatusCode::_404_NO_SUCH_BUCKET) {
            const auto bucket_create_status =
                m_s3_client->create_bucket(s3_bucket, s3_request);
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

    LOG_INFO("Doing PUT with StorageObject: " + request.object().to_string());
    LOG_INFO("Doing PUT with S3Object: " + s3_object.to_string());

    auto s3_completion_func = [completion_func, request,
                               id = m_id](S3Response::Ptr response) {
        if (!response->m_status.is_ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + " | Error putting s3 object: "
                + response->m_status.to_string());
        }
        completion_func(ObjectStoreResponse::create(request, id));
    };
    m_s3_client->put_object(
        s3_object, s3_bucket, s3_request, stream, s3_completion_func,
        request.get_progress_interval(), progress_func);
}

void S3ObjectStoreClient::get(
    const ObjectStoreRequest& request,
    completionFunc completion_func,
    Stream* stream,
    Stream::progressFunc progress_func) const
{
    S3Request s3_request;
    s3_request.m_range =
        S3Range(request.extent().m_offset, request.extent().m_length);

    S3Object s3_object;

    m_object_adapter->to_s3(s3_object, s3_request, request.object());

    S3Bucket s3_bucket(s3_object.m_bucket);

    LOG_INFO("Doing GET with StorageObject: " + request.object().to_string());
    LOG_INFO("Doing GET with S3Object: " + s3_object.to_string());

    StorageObject object;
    m_object_adapter->from_s3(object, s3_bucket, s3_object);

    auto s3_completion_func = [completion_func, object, request,
                               id = m_id](S3Response::Ptr s3_response) {
        if (!s3_response->m_status.is_ok()) {
            throw std::runtime_error(
                SOURCE_LOC() + " | Error getting s3 object: "
                + s3_response->m_status.to_string());
        }

        auto response      = ObjectStoreResponse::create(request, id);
        response->object() = object;
        completion_func(std::move(response));
    };
    m_s3_client->get_object(
        s3_object, s3_bucket, s3_request, stream, s3_completion_func,
        request.get_progress_interval(), progress_func);
}

void S3ObjectStoreClient::remove(const StorageObject& object) const
{
    S3Request s3_request;

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