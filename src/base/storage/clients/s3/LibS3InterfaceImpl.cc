#include "LibS3InterfaceImpl.h"

#include "ErrorUtils.h"
#include "Logger.h"

#include <libs3.h>
#include <stdexcept>

namespace hestia {

LibS3InterfaceImpl::~LibS3InterfaceImpl()
{
    S3_deinitialize();
}

void LibS3InterfaceImpl::initialize(const S3Config& config)
{
    m_config = config;

    LOG_INFO("Intializing s3 host to: " << config.get_default_host());

    auto status = S3_initialize(
        config.get_user_agent().c_str(), S3_INIT_ALL,
        config.get_default_host().c_str());
    if (status != S3Status::S3StatusOK) {
        const std::string msg = SOURCE_LOC() + " | Failed to initialize libs3: "
                                + S3_get_status_name(status);
        throw std::runtime_error(msg);
    }
}

struct CallbackContext {

    bool is_ok() const { return m_response_status == S3Status::S3StatusOK; }

    std::string get_error_string() const
    {
        return m_status_name + "\n" + m_error_details;
    }

    void set_status(S3Status status)
    {
        m_response_status = status;
        m_status_name     = S3_get_status_name(status);
    }

    void set_error_details(const S3ErrorDetails* error_details)
    {
        if (error_details == nullptr) {
            return;
        }
        if (error_details->message != nullptr) {
            m_error_details +=
                "Message: " + std::string(error_details->message) + '\n';
        }

        if (error_details->resource != nullptr) {
            m_error_details +=
                "Resource: " + std::string(error_details->resource) + '\n';
        }

        if (error_details->furtherDetails != nullptr) {
            m_error_details +=
                "Further Details: " + std::string(error_details->furtherDetails)
                + '\n';
        }

        for (int idx = 0; idx < error_details->extraDetailsCount; idx++) {
            m_error_details +=
                std::string(error_details->extraDetails[idx].name) + ": ";
            m_error_details +=
                std::string(error_details->extraDetails[idx].value) + '\n';
        }
    }

    S3Status m_response_status{S3Status::S3StatusOK};
    std::string m_status_name;
    std::string m_error_details;
    Stream* m_stream{nullptr};
};

std::string LibS3InterfaceImpl::get_bucket_name(const S3Object& obj) const
{
    return obj.m_container.empty() ? m_config.get_default_bucket_name() :
                                     obj.m_container;
}

void LibS3InterfaceImpl::validate_bucket_name(const std::string& name) const
{
    const auto uri_style =
        m_config.is_path_uri_style() ? S3UriStylePath : S3UriStyleVirtualHost;
    const auto status = S3_validate_bucket_name(name.c_str(), uri_style);
    if (status != S3Status::S3StatusOK) {
        const std::string status_str = S3_get_status_name(status);
        throw std::runtime_error(
            SOURCE_LOC()
            + " | Invalid Bucket name given with error: " + status_str);
    }
}

void LibS3InterfaceImpl::prepare_bucket_context(
    S3BucketContext& bucket_context,
    const std::string& name,
    const std::string& access_id,
    const std::string& access_token,
    const S3Object& obj) const
{
    bucket_context.hostName    = m_config.get_default_host().c_str();
    bucket_context.accessKeyId = access_id.c_str();
    bucket_context.authRegion =
        obj.m_region.empty() ? nullptr : obj.m_region.c_str();
    bucket_context.bucketName      = name.c_str();
    bucket_context.protocol        = S3ProtocolHTTP;
    bucket_context.secretAccessKey = access_token.c_str();
    bucket_context.securityToken   = nullptr;
    bucket_context.uriStyle =
        m_config.is_path_uri_style() ? S3UriStylePath : S3UriStyleVirtualHost;
}

bool LibS3InterfaceImpl::test_bucket(
    const std::string& name, const S3Object& obj) const
{
    LOG_INFO("Testing bucket: " << name);
    auto uri_style =
        m_config.is_path_uri_style() ? S3UriStylePath : S3UriStyleVirtualHost;

    CallbackContext cb_context;

    auto on_response_properties = [](const S3ResponseProperties* properties,
                                     void* callback_data) {
        (void)callback_data;
        LOG_INFO("Got response with length: " << properties->contentLength);
        return S3Status::S3StatusOK;
    };

    auto on_response_complete = [](S3Status status,
                                   const S3ErrorDetails* error_details,
                                   void* callback_data) {
        auto callback_context =
            reinterpret_cast<CallbackContext*>(callback_data);
        callback_context->set_status(status);
        callback_context->set_error_details(error_details);
    };

    S3ResponseHandler response_handler;
    response_handler.completeCallback   = on_response_complete;
    response_handler.propertiesCallback = on_response_properties;

    std::string location_constraint(255, '\0');

    std::string user_id = obj.m_user;
    if (auto config_id = m_config.get_access_key_id(); !config_id.empty()) {
        user_id = config_id;
    }

    std::string user_secret = obj.m_user_token;
    if (auto config_secret = m_config.get_secret_access_key();
        !config_secret.empty()) {
        user_secret = config_secret;
    }

    S3_test_bucket(
        S3ProtocolHTTP, uri_style, user_id.c_str(), user_secret.c_str(),
        nullptr, m_config.get_default_host().c_str(), name.c_str(),
        obj.m_region.empty() ? nullptr : obj.m_region.c_str(),
        location_constraint.size(), location_constraint.data(), nullptr, 0,
        &response_handler, reinterpret_cast<void*>(&cb_context));

    if (cb_context.is_ok()) {
        return true;
    }
    else if (
        cb_context.m_response_status == S3Status::S3StatusErrorNoSuchBucket) {
        return false;
    }
    else {
        const std::string msg = SOURCE_LOC() + " | Error testing S3 bucket: "
                                + cb_context.get_error_string();
        throw std::runtime_error(msg);
    }
}

void LibS3InterfaceImpl::create_bucket(
    const std::string& name, const S3Object& obj) const
{
    LOG_INFO("Creating bucket: " << name);

    CallbackContext cb_context;
    cb_context.m_response_status = S3Status::S3StatusOK;

    auto on_response_properties = [](const S3ResponseProperties* properties,
                                     void* callback_data) {
        (void)callback_data;
        LOG_INFO("Got response with length: " << properties->contentLength);
        return S3Status::S3StatusOK;
    };

    auto on_response_complete = [](S3Status status,
                                   const S3ErrorDetails* error_details,
                                   void* callback_data) {
        auto callback_context =
            reinterpret_cast<CallbackContext*>(callback_data);
        callback_context->set_status(status);
        callback_context->set_error_details(error_details);
    };

    S3ResponseHandler response_handler;
    response_handler.completeCallback   = on_response_complete;
    response_handler.propertiesCallback = on_response_properties;

    std::string user_id = obj.m_user;
    if (auto config_id = m_config.get_access_key_id(); !config_id.empty()) {
        user_id = config_id;
    }

    std::string user_secret = obj.m_user_token;
    if (auto config_secret = m_config.get_secret_access_key();
        !config_secret.empty()) {
        user_secret = config_secret;
    }

    S3_create_bucket(
        S3ProtocolHTTP, user_id.c_str(), user_secret.c_str(), nullptr,
        m_config.get_default_host().c_str(), name.c_str(),
        obj.m_region.empty() ? nullptr : obj.m_region.c_str(),
        S3CannedAcl::S3CannedAclPublicReadWrite, nullptr, nullptr, 0,
        &response_handler, reinterpret_cast<void*>(&cb_context));

    if (!cb_context.is_ok()) {
        const std::string msg = SOURCE_LOC()
                                + " | Failed to created bucket with error: "
                                + cb_context.get_error_string();
        throw std::runtime_error(msg);
    }
    else {
        LOG_INFO("Created bucket ok");
    }
}

int LibS3InterfaceImpl::put(const S3Object& obj, const Extent&, Stream* stream)
{
    const auto bucket_name = get_bucket_name(obj);
    validate_bucket_name(bucket_name);

    const auto bucket_exists = test_bucket(bucket_name, obj);
    if (!bucket_exists) {
        create_bucket(bucket_name, obj);
    }

    std::string user_id = obj.m_user;
    if (auto config_id = m_config.get_access_key_id(); !config_id.empty()) {
        user_id = config_id;
    }

    std::string user_secret = obj.m_user_token;
    if (auto config_secret = m_config.get_secret_access_key();
        !config_secret.empty()) {
        user_secret = config_secret;
    }

    S3BucketContext bucket_context;
    prepare_bucket_context(
        bucket_context, bucket_name, user_id, user_secret, obj);

    CallbackContext cb_context;
    cb_context.m_response_status = S3Status::S3StatusOK;
    cb_context.m_stream          = stream;

    auto on_response_properties = [](const S3ResponseProperties* properties,
                                     void* callback_data) {
        (void)callback_data;
        LOG_INFO("Got response with length: " << properties->contentLength);
        return S3Status::S3StatusOK;
    };

    auto on_response_complete = [](S3Status status,
                                   const S3ErrorDetails* error_details,
                                   void* callback_data) {
        auto callback_context =
            reinterpret_cast<CallbackContext*>(callback_data);
        callback_context->set_status(status);
        callback_context->set_error_details(error_details);
    };

    auto on_put_data = [](int buffer_size, char* buffer,
                          void* callback_data) -> int {
        auto cb_context = reinterpret_cast<CallbackContext*>(callback_data);
        WriteableBufferView buffer_view(buffer, buffer_size);
        const auto io_result = cb_context->m_stream->read(buffer_view);
        return io_result.m_num_transferred;
    };

    S3PutObjectHandler put_handler;
    put_handler.responseHandler.propertiesCallback = on_response_properties;
    put_handler.responseHandler.completeCallback   = on_response_complete;
    put_handler.putObjectDataCallback              = on_put_data;

    LOG_INFO("Starting LibS3 put");
    S3_put_object(
        &bucket_context, obj.m_name.c_str(), stream->get_source_size(), nullptr,
        nullptr, 0, &put_handler, reinterpret_cast<void*>(&cb_context));

    if (!cb_context.is_ok()) {
        const std::string msg = SOURCE_LOC() + " | Error in Libs3 PUT: "
                                + cb_context.get_error_string();
        throw std::runtime_error(msg);
    }

    LOG_INFO(
        "Finished LibS3 put - status is: "
        + std::string(S3_get_status_name(cb_context.m_response_status)));

    return static_cast<int>(!(cb_context.is_ok()));
}

int LibS3InterfaceImpl::get(const S3Object& obj, const Extent&, Stream* stream)
{
    const auto bucket_name = get_bucket_name(obj);
    validate_bucket_name(bucket_name);

    const auto bucket_exists = test_bucket(bucket_name, obj);
    if (!bucket_exists) {
        throw std::runtime_error(
            "Bucket not found when attempting to get object");
    }

    std::string user_id = obj.m_user;
    if (auto config_id = m_config.get_access_key_id(); !config_id.empty()) {
        user_id = config_id;
    }

    std::string user_secret = obj.m_user_token;
    if (auto config_secret = m_config.get_secret_access_key();
        !config_secret.empty()) {
        user_secret = config_secret;
    }

    S3BucketContext bucket_context;
    prepare_bucket_context(
        bucket_context, bucket_name, user_id, user_secret, obj);

    CallbackContext cb_context;
    cb_context.m_response_status = S3Status::S3StatusOK;
    cb_context.m_stream          = stream;

    auto on_response_properties = [](const S3ResponseProperties* properties,
                                     void* callback_data) {
        (void)callback_data;
        LOG_INFO("Got response with length: " << properties->contentLength);
        return S3Status::S3StatusOK;
    };

    auto on_response_complete = [](S3Status status,
                                   const S3ErrorDetails* error_details,
                                   void* callback_data) {
        auto callback_context =
            reinterpret_cast<CallbackContext*>(callback_data);
        callback_context->set_status(status);
        callback_context->set_error_details(error_details);
    };

    auto on_get_data = [](int buffer_size, const char* buffer,
                          void* callback_data) {
        LOG_INFO("Getting buffer with size: " << buffer_size);

        auto cb_context = reinterpret_cast<CallbackContext*>(callback_data);
        ReadableBufferView buffer_view(buffer, buffer_size);
        const auto io_result = cb_context->m_stream->write(buffer_view);
        return io_result.ok() ? S3Status::S3StatusOK :
                                S3Status::S3StatusErrorUnknown;
    };

    S3GetObjectHandler get_handler;
    get_handler.responseHandler.propertiesCallback = on_response_properties;
    get_handler.responseHandler.completeCallback   = on_response_complete;
    get_handler.getObjectDataCallback              = on_get_data;

    LOG_INFO("Doing S3 Get operation");

    S3_get_object(
        &bucket_context, obj.m_name.c_str(), nullptr, 0,
        stream->get_sink_size(), nullptr, 0, &get_handler,
        reinterpret_cast<void*>(&cb_context));

    if (!cb_context.is_ok()) {
        const std::string msg = SOURCE_LOC() + " | Error in Libs3 GET: "
                                + cb_context.get_error_string();
        throw std::runtime_error(msg);
    }

    return static_cast<int>(
        !(cb_context.m_response_status == S3Status::S3StatusOK));
}

int LibS3InterfaceImpl::remove(const S3Object& obj)
{
    const auto bucket_name = get_bucket_name(obj);
    validate_bucket_name(bucket_name);

    const auto bucket_exists = test_bucket(bucket_name, obj);
    if (!bucket_exists) {
        throw std::runtime_error(
            "Bucket not found when attempting to remove object");
    }

    std::string user_id = obj.m_user;
    if (auto config_id = m_config.get_access_key_id(); !config_id.empty()) {
        user_id = config_id;
    }

    std::string user_secret = obj.m_user_token;
    if (auto config_secret = m_config.get_secret_access_key();
        !config_secret.empty()) {
        user_secret = config_secret;
    }

    S3BucketContext bucket_context;
    prepare_bucket_context(
        bucket_context, bucket_name, user_id, user_secret, obj);

    CallbackContext cb_context;
    cb_context.m_response_status = S3Status::S3StatusOK;

    auto on_response_properties = [](const S3ResponseProperties* properties,
                                     void* callback_data) {
        (void)callback_data;
        LOG_INFO("Got response with length: " << properties->contentLength);
        return S3Status::S3StatusOK;
    };

    auto on_response_complete = [](S3Status status,
                                   const S3ErrorDetails* error_details,
                                   void* callback_data) {
        auto callback_context =
            reinterpret_cast<CallbackContext*>(callback_data);
        callback_context->set_status(status);
        callback_context->set_error_details(error_details);
    };

    S3ResponseHandler response_handler;
    response_handler.completeCallback   = on_response_complete;
    response_handler.propertiesCallback = on_response_properties;

    S3_delete_object(
        &bucket_context, obj.m_name.c_str(), nullptr, 0, &response_handler,
        reinterpret_cast<void*>(&cb_context));

    if (!cb_context.is_ok()) {
        const std::string msg = SOURCE_LOC() + " | Error in Libs3 Delete: "
                                + cb_context.get_error_string();
        throw std::runtime_error(msg);
    }

    return static_cast<int>(
        !(cb_context.m_response_status == S3Status::S3StatusOK));
}
}  // namespace hestia
