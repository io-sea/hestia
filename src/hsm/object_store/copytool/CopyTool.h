#pragma once

#include "CopyToolConfig.h"
#include "HsmObjectStoreClientFactory.h"

#include <memory>
#include <string>

namespace hestia {
class ObjectStoreClient;

class HsmObjectStoreClientManager;
class HsmObjectStoreClient;

class CopyTool {
  public:
    CopyTool(std::unique_ptr<HsmObjectStoreClientManager> client_manager);

    ~CopyTool();

    void initialize();

    HsmObjectStoreResponse::Ptr make_object_store_request(
        const HsmObjectStoreRequest& request) noexcept;

  private:
    HsmObjectStoreResponse::Ptr do_hsm_hsm(
        const HsmObjectStoreRequest& request,
        HsmObjectStoreClient* source_client,
        HsmObjectStoreClient* target_client) noexcept;

    HsmObjectStoreResponse::Ptr do_hsm_base(
        const HsmObjectStoreRequest& request,
        HsmObjectStoreClient* source_client,
        ObjectStoreClient* target_client) noexcept;

    HsmObjectStoreResponse::Ptr do_base_hsm(
        const HsmObjectStoreRequest& request,
        ObjectStoreClient* source_client,
        HsmObjectStoreClient* target_client) noexcept;

    HsmObjectStoreResponse::Ptr do_base_base(
        const HsmObjectStoreRequest& request,
        ObjectStoreClient* source_client,
        ObjectStoreClient* target_client) noexcept;

    std::unique_ptr<HsmObjectStoreClientManager> m_client_manager;
};
}  // namespace hestia