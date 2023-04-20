#pragma once

#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientRegistry.h"
#include "CopyToolConfig.h"

#include <memory>

class HsmObjectStoreClientManager;
class CopyToolInterface;

class MultiBackendHsmObjectStoreClient : public HsmObjectStoreClient
{
public:
    using Ptr = std::unique_ptr<MultiBackendHsmObjectStoreClient>;

    MultiBackendHsmObjectStoreClient(std::unique_ptr<HsmObjectStoreClientManager> clientManager);

    ~MultiBackendHsmObjectStoreClient();

    void initialize(const TierBackendRegistry& tierBackendRegsitry, const CopyToolConfig& copyToolConfig);

    [[nodiscard]] HsmObjectStoreResponse::Ptr makeRequest(const HsmObjectStoreRequest& request, ostk::Stream* stream = nullptr) const noexcept override;

private:
    void copy(const HsmObjectStoreRequest& request) const override{};

    void get(const HsmObjectStoreRequest& request, ostk::StorageObject& object, ostk::Stream* stream) const override{};

    void move(const HsmObjectStoreRequest& request) const override{};

    void put(const HsmObjectStoreRequest& request, ostk::Stream* stream) const override{};

    void remove(const HsmObjectStoreRequest& request) const override{};

    std::unique_ptr<CopyToolInterface> mCopyToolInterface;
    std::unique_ptr<HsmObjectStoreClientManager> mClientManager;
};