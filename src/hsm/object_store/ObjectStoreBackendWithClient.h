#pragma once

#include "HsmObjectStoreClient.h"
#include "HsmObjectStoreClientPlugin.h"
#include "ObjectStoreBackend.h"
#include "ObjectStoreClientPlugin.h"

namespace hestia {

class ObjectStoreBackendWithClient {
  public:
    using Ptr = std::unique_ptr<ObjectStoreBackendWithClient>;

    ObjectStoreBackendWithClient(const ObjectStoreBackend& backend);

    static Ptr create(const ObjectStoreBackend& backend);

    void make_request(HsmObjectStoreContext& ctx) const noexcept;

    ObjectStoreClient::Ptr m_object_store_client;
    ObjectStoreClientPlugin::Ptr m_object_store_client_plugin;
    HsmObjectStoreClient::Ptr m_hsm_object_store_client;
    HsmObjectStoreClientPlugin::Ptr m_hsm_object_store_client_plugin;
    ObjectStoreBackend m_backend;
};
}  // namespace hestia