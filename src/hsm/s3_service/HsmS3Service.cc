#include "HsmS3Service.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "KeyValueStoreClient.h"
#include "S3ContainerAdapter.h"
#include "S3ObjectAdapter.h"

#include "Logger.h"

namespace hestia {
HsmS3Service::HsmS3Service(
    DistributedHsmService* hsm_service, KeyValueStoreClient* kv_store_client) :
    S3Service({}),
    m_hsm_service(hsm_service),
    m_kv_store_client(kv_store_client)
{
    LOG_INFO("Creating HsmS3Service");
}

HsmS3Service::~HsmS3Service()
{
    LOG_INFO("Destroying HsmS3Service");
}

std::pair<S3Service::Status, bool> HsmS3Service::exists(
    const S3Container& container) const noexcept
{
    LOG_INFO("Starting exists request for container: " << container.m_name);
    const auto status = exists(container.m_name);
    LOG_INFO("Finished exists request: exists " << status.second);
    return status;
}

[[nodiscard]] std::pair<HsmS3Service::Status, bool> HsmS3Service::exists(
    const S3Object& object) const noexcept
{
    LOG_INFO("Starting exists request for object: " << object.m_key);
    const auto status = exists(object.m_key);
    LOG_INFO("Finished exists request: exists " << status.second);
    return status;
}

std::pair<S3Service::Status, bool> HsmS3Service::exists(
    const std::string& object_id) const
{
    HsmServiceRequest request(
        StorageObject(object_id), HsmServiceRequestMethod::EXISTS);
    if (const auto response =
            m_hsm_service->get_hsm_service()->make_request(request);
        response->ok()) {
        return {{}, response->object_found()};
    }
    else {
        LOG_ERROR(
            "Error in exists check: " << response->get_error().to_string());
        return {
            {S3Service::Status::Code::ERROR, response->get_error().to_string()},
            false};
    }
}

S3Service::Status HsmS3Service::get(S3Container& container) const noexcept
{
    LOG_INFO("Starting container get request for: " << container.m_name);

    HsmServiceRequest request(
        StorageObject(container.m_name), HsmServiceRequestMethod::GET);
    if (const auto response =
            m_hsm_service->get_hsm_service()->make_request(request);
        response->ok()) {
        m_container_adapter->to_s3(container, response->object().object());
    }
    else {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }
    LOG_INFO("Finished container get request");
    return {};
}

[[nodiscard]] HsmS3Service::Status HsmS3Service::get(
    S3Object& object, const Extent& extent, Stream* stream) const noexcept
{
    LOG_INFO("Starting object get request for: " << object.m_key);
    HsmServiceRequest request(object.m_key, HsmServiceRequestMethod::GET);
    request.set_extent(extent);

    const std::string tier_key = "hestia:tier";
    auto tier_str              = object.m_user_metadata.get_item(tier_key);
    uint8_t tier               = 0;
    if (!tier_str.empty()) {
        tier = std::stoul(tier_str);
    }
    request.set_target_tier(tier);

    if (const auto response =
            m_hsm_service->get_hsm_service()->make_request(request, stream);
        response->ok()) {
        m_object_adapter->to_s3(object, response->object().object());
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

[[nodiscard]] S3Service::Status HsmS3Service::put(
    const S3Container& container) noexcept
{
    LOG_INFO("Starting container put request for: " << container.m_name);

    HsmServiceRequest request(
        StorageObject(container.m_name), HsmServiceRequestMethod::PUT);
    m_container_adapter->from_s3(request.object(), container);

    if (const auto response =
            m_hsm_service->get_hsm_service()->make_request(request);
        !response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    const std::string key = "hestia:buckets";
    const Metadata::Query query{key, container.m_name};

    KeyValueStoreRequest kv_set_add_request(
        KeyValueStoreRequestMethod::SET_ADD, query);
    const auto response = m_kv_store_client->make_request(kv_set_add_request);
    if (!response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    LOG_INFO("Finished container put request");
    return {};
}

[[nodiscard]] S3Service::Status HsmS3Service::put(
    const S3Container& container,
    const S3Object& object,
    const Extent& extent,
    Stream* stream) noexcept
{
    LOG_INFO(
        "Starting object put request for: " << object.m_key << " in container "
                                            << container.m_name);

    HsmServiceRequest request(
        StorageObject(container.m_name), HsmServiceRequestMethod::PUT);
    request.set_extent(extent);

    m_object_adapter->from_s3(request.object(), container, object);
    const std::string tier_key = "hestia:tier";
    auto tier_str              = object.m_user_metadata.get_item(tier_key);
    uint8_t tier               = 0;
    if (!tier_str.empty()) {
        tier = std::stoul(tier_str);
    }
    request.set_target_tier(tier);

    if (const auto response =
            m_hsm_service->get_hsm_service()->make_request(request, stream);
        !response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    const std::string key = "hestia:bucket:" + container.m_name + ":objects";
    const Metadata::Query query{key, object.m_key};

    KeyValueStoreRequest kv_set_add_request(
        KeyValueStoreRequestMethod::SET_ADD, query);
    const auto response = m_kv_store_client->make_request(kv_set_add_request);
    if (!response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    LOG_INFO("Finished object put request");
    return {};
}

S3Service::Status HsmS3Service::list(std::vector<S3Container>& fetched) const
{
    LOG_INFO("Starting container list request");

    const std::string key = "hestia:buckets";

    const auto response = m_kv_store_client->make_request(
        {KeyValueStoreRequestMethod::SET_LIST, key});
    if (!response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    for (const auto& id : response->ids()) {
        fetched.push_back(S3Container(id));
    }
    LOG_INFO("Finished container list request: found " << fetched.size());
    return {};
}

S3Service::Status HsmS3Service::list(
    const S3Container& container, std::vector<S3Object>& fetched) const
{
    LOG_INFO(
        "Starting object list request for container: " << container.m_name);

    const std::string key = "hestia:bucket:" + container.m_name + ":objects";
    KeyValueStoreRequest request(KeyValueStoreRequestMethod::SET_LIST, key);
    const auto response = m_kv_store_client->make_request(request);
    if (!response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    for (const auto& id : response->ids()) {
        fetched.push_back(S3Object(id));
    }
    LOG_INFO("Finished object list request: found " << fetched.size());
    return {};
}

}  // namespace hestia
