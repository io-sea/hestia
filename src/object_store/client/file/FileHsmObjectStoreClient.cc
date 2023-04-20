#include "FileHsmObjectStoreClient.h"

#include <ostk/RequestException.h>
#include <ostk/FileUtils.h>
#include <ostk/FileObjectStoreClient.h>

FileHsmObjectStoreClient::FileHsmObjectStoreClient()
{

}

FileHsmObjectStoreClient::~FileHsmObjectStoreClient()
{

}

std::string FileHsmObjectStoreClient::getRegistryIdentifier()
{
    return "hestia::FileHsmObjectStoreClient";
}

FileHsmObjectStoreClient::Ptr FileHsmObjectStoreClient::Create()
{
    return std::make_unique<FileHsmObjectStoreClient>();
}

void FileHsmObjectStoreClient::initialize(const FileHsmObjectStoreClientConfig& config)
{
    mStore = config.mStoreLocation;
    if (!mStore.exists()) 
    {
        std::filesystem::create_directories(mStore.path());
    }
}

std::filesystem::path FileHsmObjectStoreClient::getTierPath(uint8_t tier) const
{
    return mStore.path() / ("tier" + std::to_string(tier));
}

void FileHsmObjectStoreClient::put(const HsmObjectStoreRequest& request, ostk::Stream* stream) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(getTierPath(request.targetTier()));
    if (const auto response = file_client.makeRequest(HsmObjectStoreRequest::toBaseRequest(request), stream); !response->ok())
    {
        const std::string msg = "Error in file client PUT: " + response->getError().toString();
        throw ostk::RequestException<HsmObjectStoreError>({HsmObjectStoreErrorCode::ERROR, msg});
    }
}

void FileHsmObjectStoreClient::get(const HsmObjectStoreRequest& request, ostk::StorageObject& object, ostk::Stream* stream) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(getTierPath(request.sourceTier()));
    if (const auto response = file_client.makeRequest(HsmObjectStoreRequest::toBaseRequest(request), stream); !response->ok())
    {
        const std::string msg = "Error in file client GET: " + response->getError().toString();
        throw ostk::RequestException<HsmObjectStoreError>({HsmObjectStoreErrorCode::ERROR, msg});
    }
    else
    {
        object = response->object();
    }
}

void FileHsmObjectStoreClient::remove(const HsmObjectStoreRequest& request) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(getTierPath(request.sourceTier()));
    if (const auto response = file_client.makeRequest(HsmObjectStoreRequest::toBaseRequest(request)); !response->ok())
    {
        const std::string msg = "Error in file client REMOVE: " + response->getError().toString();
        throw ostk::RequestException<HsmObjectStoreError>({HsmObjectStoreErrorCode::ERROR, msg});
    }
}

void FileHsmObjectStoreClient::copy(const HsmObjectStoreRequest& request) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(getTierPath(request.sourceTier()));
    file_client.migrate(request.object().id(), getTierPath(request.targetTier()), true);
}

void FileHsmObjectStoreClient::move(const HsmObjectStoreRequest& request) const
{
    ostk::FileObjectStoreClient file_client;
    file_client.initialize(getTierPath(request.sourceTier()));
    file_client.migrate(request.object().id(), getTierPath(request.targetTier()), false);
}