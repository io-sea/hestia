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
    m_store = config.mStoreLocation;
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
    file_client.initialize(get_tier_path(request.targetTier()));
    if (const auto response = file_client.makeRequest(
            HsmObjectStoreRequest::toBaseRequest(request), stream);
        !response->ok()) {
        const std::string msg =
            "Error in file client PUT: " + response->getError().toString();
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
    file_client.initialize(get_tier_path(request.sourceTier()));
    if (const auto response = file_client.makeRequest(
            HsmObjectStoreRequest::toBaseRequest(request), stream);
        !response->ok()) {
        const std::string msg =
            "Error in file client GET: " + response->getError().toString();
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
    file_client.initialize(get_tier_path(request.sourceTier()));
    if (const auto response = file_client.makeRequest(
            HsmObjectStoreRequest::toBaseRequest(request));
        !response->ok()) {
        const std::string msg =
            "Error in file client REMOVE: " + response->getError().toString();
        throw ostk::RequestException<HsmObjectStoreError>(
            {HsmObjectStoreErrorCode::ERROR, msg});
    }
}

void FileHsmObjectStoreClient::copy(const HsmObjectStoreRequest& request) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(get_tier_path(request.sourceTier()));
    file_client.migrate(
        request.object().id(), get_tier_path(request.targetTier()), true);
}

void FileHsmObjectStoreClient::move(const HsmObjectStoreRequest& request) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(get_tier_path(request.sourceTier()));
    file_client.migrate(
        request.object().id(), get_tier_path(request.targetTier()), false);
}