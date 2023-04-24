#include "FileHsmObjectStoreClient.h"

#include <ostk/FileObjectStoreClient.h>
#include <ostk/FileUtils.h>
#include <ostk/RequestException.h>

FileHsmObjectStoreClient::FileHsmObjectStoreClient() {}

FileHsmObjectStoreClient::~FileHsmObjectStoreClient() {}

std::string FileHsmObjectStoreClient::get_registry_identifier()
{
    return "hestia::FileHsmObjectStoreClient";
}

FileHsmObjectStoreClient::Ptr FileHsmObjectStoreClient::create()
{
    return std::make_unique<FileHsmObjectStoreClient>();
}

void FileHsmObjectStoreClient::initialize(
    const FileHsmObjectStoreClientConfig& config)
{
    m_store = config.m_store_location;
    if (!m_store.exists()) {
        std::filesystem::create_directories(m_store.path());
    }
}

std::filesystem::path FileHsmObjectStoreClient::get_tier_path(
    uint8_t tier) const
{
    return m_store.path() / ("tier" + std::to_string(tier));
}

void FileHsmObjectStoreClient::put(
    const HsmObjectStoreRequest& request, ostk::Stream* stream) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(get_tier_path(request.target_tier()));
    if (const auto response = file_client.make_request(
            HsmObjectStoreRequest::to_base_request(request), stream);
        !response->ok()) {
        const std::string msg =
            "Error in file client PUT: " + response->get_error().to_string();
        throw ostk::RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
}

void FileHsmObjectStoreClient::get(
    const HsmObjectStoreRequest& request,
    ostk::StorageObject& object,
    ostk::Stream* stream) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(get_tier_path(request.source_tier()));
    if (const auto response = file_client.make_request(
            HsmObjectStoreRequest::to_base_request(request), stream);
        !response->ok()) {
        const std::string msg =
            "Error in file client GET: " + response->get_error().to_string();
        throw ostk::RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
    else {
        object = response->object();
    }
}

void FileHsmObjectStoreClient::remove(
    const HsmObjectStoreRequest& request) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(get_tier_path(request.source_tier()));
    if (const auto response = file_client.make_request(
            HsmObjectStoreRequest::to_base_request(request));
        !response->ok()) {
        const std::string msg =
            "Error in file client REMOVE: " + response->get_error().to_string();
        throw ostk::RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
}

void FileHsmObjectStoreClient::copy(const HsmObjectStoreRequest& request) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(get_tier_path(request.source_tier()));
    file_client.migrate(
        request.object().id(), get_tier_path(request.target_tier()), true);
}

void FileHsmObjectStoreClient::move(const HsmObjectStoreRequest& request) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(get_tier_path(request.source_tier()));
    file_client.migrate(
        request.object().id(), get_tier_path(request.target_tier()), false);
}