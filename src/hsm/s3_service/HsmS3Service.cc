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
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::SET_LIST, query);
    const auto response = m_kv_store_client->make_request(request);
    if (!response->ok()) {
        return {
            S3Service::Status::Code::ERROR, response->get_error().to_string()};
    }

    for (const auto& item : response->items()) {
        fetched.push_back(S3Object(item));
    }
    LOG_INFO("Finished object list request: found " << fetched.size());
    return {};
}

}  // namespace hestia
