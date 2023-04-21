#pragma once

#include "CopyToolConfig.h"
#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientRegistry.h"

#include <memory>

class HsmObjectStoreClientManager;
class CopyToolInterface;

class MultiBackendHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<MultiBackendHsmObjectStoreClient>;

    MultiBackendHsmObjectStoreClient(
        std::unique_ptr<HsmObjectStoreClientManager> client_manager);

    ~MultiBackendHsmObjectStoreClient();

    void initialize(
        const TierBackendRegistry& tier_backend_regsitry,
        const CopyToolConfig& copy_tool_config);

    [[nodiscard]] HsmObjectStoreResponse::Ptr makeRequest(
        const HsmObjectStoreRequest& request,
        ostk::Stream* stream = nullptr) const noexcept override;

  private:
    void copy(const HsmObjectStoreRequest& request) const override{};

    void get(
        const HsmObjectStoreRequest& request,
        ostk::StorageObject& object,
        ostk::Stream* stream) const override{};

    void move(const HsmObjectStoreRequest& request) const override{};

    void put(const HsmObjectStoreRequest& request, ostk::Stream* stream)
        const override{};

    void remove(const HsmObjectStoreRequest& request) const override{};

    std::unique_ptr<CopyToolInterface> m_copy_tool_interface;
    std::unique_ptr<HsmObjectStoreClientManager> m_client_manager;
};