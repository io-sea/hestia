#include "CopyTool.h"

#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientManager.h"

#include <ostk/Stream.h>

CopyTool::CopyTool(HsmObjectStoreClientManager::Ptr client_manager) :
    mClientManager(std::move(clientManager))
{
}

CopyTool::~CopyTool() {}

void CopyTool::initialize() {}

HsmObjectStoreResponse::Ptr CopyTool::make_object_store_request(
    const HsmObjectStoreRequest& request) noexcept
{
    if (mClientManager->isHsmClient(request.sourceTier())) {
        if (mClientManager->isHsmClient(request.targetTier())) {
            return doHsmHsm(
                request, mClientManager->getHsmClient(request.sourceTier()),
                mClientManager->getHsmClient(request.targetTier()));
        }
        else {
            return doHsmBase(
                request, mClientManager->getHsmClient(request.sourceTier()),
                mClientManager->getClient(request.targetTier()));
        }
    }
    else {
        if (mClientManager->isHsmClient(request.targetTier())) {
            return doBaseHsm(
                request, mClientManager->getClient(request.sourceTier()),
                mClientManager->getHsmClient(request.targetTier()));
        }
        else {
            return doBaseBase(
                request, mClientManager->getClient(request.sourceTier()),
                mClientManager->getClient(request.targetTier()));
        }
    }
};

HsmObjectStoreResponse::Ptr CopyTool::do_hsm_hsm(
    const HsmObjectStoreRequest& request,
    HsmObjectStoreClient* sourceClient,
    HsmObjectStoreClient* targetClient) noexcept
{
    std::size_t chunk_size{4000};
    ostk::Stream stream;

    HsmObjectStoreRequest get_request = request;
    get_request.resetMethod(HsmObjectStoreRequestMethod::GET);
    if (auto get_response = sourceClient->makeRequest(get_request, &stream);
        !get_response->ok()) {
        return get_response;
    }

    HsmObjectStoreRequest put_request = request;
    put_request.resetMethod(HsmObjectStoreRequestMethod::PUT);
    if (auto put_response = targetClient->makeRequest(put_request, &stream);
        !put_response->ok()) {
        return put_response;
    }

    auto response           = HsmObjectStoreResponse::Create(request);
    const auto stream_state = stream.flush();
    if (!stream_state.ok()) {
        response->onError(
            {HsmObjectStoreErrorCode::ERROR,
             "Error flushing stream: " + stream_state.message()});
    }
    return response;
}

HsmObjectStoreResponse::Ptr CopyTool::do_hsm_base(
    const HsmObjectStoreRequest& op,
    HsmObjectStoreClient* sourceClient,
    ostk::ObjectStoreClient* targetClient) noexcept
{
    return nullptr;
}

HsmObjectStoreResponse::Ptr CopyTool::do_base_hsm(
    const HsmObjectStoreRequest& op,
    ostk::ObjectStoreClient* sourceClient,
    HsmObjectStoreClient* targetClient) noexcept
{
    return nullptr;
}

HsmObjectStoreResponse::Ptr CopyTool::do_base_base(
    const HsmObjectStoreRequest& op,
    ostk::ObjectStoreClient* sourceClient,
    ostk::ObjectStoreClient* targetClient) noexcept
{
    return nullptr;
}