#pragma once

#include "CopyToolConfig.h"
#include "HsmObjectStoreClientRegistry.h"

#include <memory>
#include <string>

namespace ostk
{
    class ObjectStoreClient;
}

class HsmObjectStoreClientManager;
class HsmObjectStoreClient;

class CopyTool
{
public:
    CopyTool(std::unique_ptr<HsmObjectStoreClientManager> clientManager);

    ~CopyTool();

    void initialize();

    HsmObjectStoreResponse::Ptr makeObjectStoreRequest(const HsmObjectStoreRequest& request) noexcept;

private:
    HsmObjectStoreResponse::Ptr doHsmHsm(const HsmObjectStoreRequest& request, HsmObjectStoreClient* sourceClient, HsmObjectStoreClient* targetClient) noexcept;

    HsmObjectStoreResponse::Ptr doHsmBase(const HsmObjectStoreRequest& request, HsmObjectStoreClient* sourceClient, ostk::ObjectStoreClient* targetClient) noexcept;

    HsmObjectStoreResponse::Ptr doBaseHsm(const HsmObjectStoreRequest& request, ostk::ObjectStoreClient* sourceClient, HsmObjectStoreClient* targetClient) noexcept;

    HsmObjectStoreResponse::Ptr doBaseBase(const HsmObjectStoreRequest& request, ostk::ObjectStoreClient* sourceClient, ostk::ObjectStoreClient* targetClient) noexcept;
    
    std::unique_ptr<HsmObjectStoreClientManager> mClientManager;
};