#include "MultiBackendHsmObjectStoreClient.h"

#include "CopyToolInterface.h"
#include "HsmObjectStoreClientManager.h"
#include "HsmObjectStoreClientRegistry.h"

MultiBackendHsmObjectStoreClient::MultiBackendHsmObjectStoreClient(
    std::unique_ptr<HsmObjectStoreClientManager> clientManager) :
    mClientManager(std::move(clientManager)),
    mCopyToolInterface(std::make_unique<CopyToolInterface>())
{
}

MultiBackendHsmObjectStoreClient::~MultiBackendHsmObjectStoreClient() {}

void MultiBackendHsmObjectStoreClient::initialize(
    const TierBackendRegistry& tierBackendRegsitry,
    const CopyToolConfig& copyToolConfig)
{
    mClientManager->setupClients(tierBackendRegsitry);
    mCopyToolInterface->initialize(copyToolConfig);
}

HsmObjectStoreResponse::Ptr MultiBackendHsmObjectStoreClient::makeRequest(
    const HsmObjectStoreRequest& request, ostk::Stream* stream) const noexcept
{
    if (request.isHsmOnlyRequest()) {
        if (mClientManager->haveSameClientTypes(
                request.targetTier(), request.sourceTier())) {
            if (mClientManager->isHsmClient(request.sourceTier())) {
                auto client =
                    mClientManager->getHsmClient(request.sourceTier());
                return client->makeRequest(request);
            }
            else {
                auto client   = mClientManager->getClient(request.sourceTier());
                auto response = client->makeRequest(
                    HsmObjectStoreRequest::toBaseRequest(request));
                return HsmObjectStoreResponse::Create(
                    request, std::move(response));
            }
        }
        else {
            return HsmObjectStoreResponse::Create(
                request, mCopyToolInterface->makeRequest(request));
        }
    }
    else {
        const auto client_tier =
            request.method() == HsmObjectStoreRequestMethod::PUT ?
                request.targetTier() :
                request.sourceTier();
        if (mClientManager->isHsmClient(client_tier)) {
            auto client = mClientManager->getHsmClient(client_tier);
            return client->makeRequest(request, stream);
        }
        else {
            auto client   = mClientManager->getClient(client_tier);
            auto response = client->makeRequest(
                HsmObjectStoreRequest::toBaseRequest(request));
            return HsmObjectStoreResponse::Create(request, std::move(response));
        }
    }
}
