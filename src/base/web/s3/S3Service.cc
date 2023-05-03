#include "S3Service.h"

#include "S3ContainerAdapter.h"
#include "S3ObjectAdapter.h"

#include "Logger.h"

namespace hestia {
S3Service::S3Service(const S3ServiceConfig& config) :
    m_object_store_client(config.m_object_store_client),
    m_object_adapter(
        std::make_unique<S3ObjectAdapter>(config.m_metadata_prefix)),
    m_container_adapter(
        std::make_unique<S3ContainerAdapter>(config.m_metadata_prefix))
{
}

S3Service::~S3Service() {}

std::unique_ptr<S3Service> S3Service::create(const S3ServiceConfig& config)
{
    return std::make_unique<S3Service>(config);
}

S3Service::Status S3Service::list(std::vector<S3Container>& fetched) const
{
    LOG_INFO("Starting container list request");
    ObjectStoreRequest request(m_container_adapter->get_query_filter());
    const auto response = m_object_store_client->make_request(request);
    if (!response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    for (const auto& object : response->objects()) {
        S3Container container(object.m_id);
        m_container_adapter->to_s3(container, object);
        fetched.push_back(container);
    }
    LOG_INFO("Finished container list request: found " << fetched.size());
    return {};
}

S3Service::Status S3Service::list(
    const S3Container& container, std::vector<S3Object>& fetched) const
{
    LOG_INFO(
        "Starting object list request for container: " << container.m_name);
    std::vector<StorageObject> objects;
    ObjectStoreRequest request(
        {m_object_adapter->get_query_filter(), container.m_name});
    const auto response = m_object_store_client->make_request(request);
    if (!response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    for (const auto& object : response->objects()) {
        S3Object s3_object;
        m_object_adapter->to_s3(s3_object, object);
        fetched.push_back(s3_object);
    }
    LOG_INFO("Finished object list request: found " << fetched.size());
    return {};
}

std::pair<S3Service::Status, bool> S3Service::exists(
    const S3Container& container) const noexcept
{
    LOG_INFO("Starting exists request for container: " << container.m_name);
    const auto status = exists(container.m_name);
    LOG_INFO("Finished exists request: exists " << status.second);
    return status;
}

std::pair<S3Service::Status, bool> S3Service::exists(
    const S3Object& object) const noexcept
{
    LOG_INFO("Starting exists request for object: " << object.m_key);
    const auto status = exists(object.m_key);
    LOG_INFO("Finished exists request: exists " << status.second);
    return status;
}

std::pair<S3Service::Status, bool> S3Service::exists(
    const std::string& object_id) const
{
    ObjectStoreRequest request(object_id, ObjectStoreRequestMethod::EXISTS);
    if (const auto response = m_object_store_client->make_request(request);
        response->ok()) {
        return {{}, response->object_found()};
    }
    else {
        return {
            {S3Service::Status::Code::ERROR, response->get_error().to_string()},
            false};
    }
}

S3Service::Status S3Service::get(S3Container& container) const noexcept
{
    LOG_INFO("Starting container get request for: " << container.m_name);
    ObjectStoreRequest request(container.m_name, ObjectStoreRequestMethod::GET);
    if (const auto response = m_object_store_client->make_request(request);
        response->ok()) {
        m_container_adapter->to_s3(container, response->object());
    }
    else {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }
    LOG_INFO("Finished container get request");
    return {};
}

S3Service::Status S3Service::get(
    S3Object& object, const Extent& extent, Stream* stream) const noexcept
{
    LOG_INFO("Starting object get request for: " << object.m_key);
    ObjectStoreRequest request(object.m_key, ObjectStoreRequestMethod::GET);
    request.set_extent(extent);

    if (const auto response =
            m_object_store_client->make_request(request, stream);
        response->ok()) {
        m_object_adapter->to_s3(object, response->object());
        if (stream != nullptr) {
            stream->set_source_size(std::stoi(object.m_content_length));
        }
    }
    else {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }
    LOG_INFO("Finished object get request");
    return {};
}

S3Service::Status S3Service::put(const S3Container& container) noexcept
{
    LOG_INFO("Starting container put request for: " << container.m_name);
    ObjectStoreRequest request(container.m_name, ObjectStoreRequestMethod::PUT);
    m_container_adapter->from_s3(request.object(), container);
    if (const auto response = m_object_store_client->make_request(request);
        !response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }
    LOG_INFO("Finished container put request");
    return {};
}

S3Service::Status S3Service::put(
    const S3Container& container,
    const S3Object& object,
    const Extent& extent,
    Stream* stream) noexcept
{
    LOG_INFO("Starting object put request for: " << object.m_key);
    ObjectStoreRequest request(container.m_name, ObjectStoreRequestMethod::PUT);
    request.set_extent(extent);
    m_object_adapter->from_s3(request.object(), container, object);
    if (const auto response =
            m_object_store_client->make_request(request, stream);
        !response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }
    LOG_INFO("Finished object put request");
    return {};
}

S3Service::Status S3Service::remove(const S3Object& object) noexcept
{
    LOG_INFO("Starting object remove request for: " << object.m_key);
    const auto status = remove(object.m_key);
    LOG_INFO("Finished object remove request");
    return status;
}

S3Service::Status S3Service::remove(const S3Container& container) noexcept
{
    LOG_INFO("Starting container remove request for: " << container.m_name);
    const auto status = remove(container.m_name);
    LOG_INFO("Finished container remove request");
    return status;
}

S3Service::Status S3Service::remove(const std::string& object_id)
{
    ObjectStoreRequest request(object_id, ObjectStoreRequestMethod::REMOVE);
    if (const auto response = m_object_store_client->make_request(request);
        response->ok()) {
        return {};
    }
    else {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }
}

S3Service::Status S3Service::make_object_store_request(
    const ObjectStoreRequest& request, Stream* stream)
{
    if (const auto response =
            m_object_store_client->make_request(request, stream);
        !response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }
    return {};
}
}  // namespace hestia