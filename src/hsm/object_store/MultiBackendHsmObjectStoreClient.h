#pragma once

#include "CopyToolConfig.h"
#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientFactory.h"

#include <memory>

namespace hestia {
class HsmObjectStoreClientManager;
class CopyToolInterface;

class MultiBackendHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<MultiBackendHsmObjectStoreClient>;

    MultiBackendHsmObjectStoreClient(
        std::unique_ptr<HsmObjectStoreClientManager> client_manager);

    static Ptr create();

    virtual ~MultiBackendHsmObjectStoreClient();

    void do_initialize(
        const TierBackendRegistry& tier_backend_regsitry,
        const CopyToolConfig& copy_tool_config);

    [[nodiscard]] HsmObjectStoreResponse::Ptr make_request(
        const HsmObjectStoreRequest& request,
        Stream* stream = nullptr) const noexcept override;

  private:
    void copy(const HsmObjectStoreRequest& request) const override
    {
        (void)request;
    };

    void get(
        const HsmObjectStoreRequest& request,
        StorageObject& object,
        Stream* stream) const override
    {
        (void)request;
        (void)object, (void)stream;
    };

    void move(const HsmObjectStoreRequest& request) const override
    {
        (void)request;
    };

    void put(
        const HsmObjectStoreRequest& request, Stream* stream) const override
    {
        (void)request;
        (void)stream;
    };

    void remove(const HsmObjectStoreRequest& request) const override
    {
        (void)request;
    };

    std::unique_ptr<CopyToolInterface> m_copy_tool_interface;
    std::unique_ptr<HsmObjectStoreClientManager> m_client_manager;
};
}  // namespace hestia