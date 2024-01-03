#pragma once

#include "HsmObjectStoreClient.h"
#include "ObjectStorePluginHandler.h"

namespace hestia {

class S3Client;

class ObjectStoreClientFactory {
  public:
    using Ptr = std::unique_ptr<ObjectStoreClientFactory>;
    ObjectStoreClientFactory(ObjectStorePluginHandler::Ptr plugin_handler);

    static Ptr create(ObjectStorePluginHandler::Ptr plugin_handler);

    bool is_client_type_available(const ObjectStoreBackend& client_spec) const;

    ObjectStoreClient::Ptr get_client(
        const ObjectStoreBackend& client_spec, S3Client* s3_client) const;

    ObjectStoreClientPlugin::Ptr get_client_from_plugin(
        const ObjectStoreBackend& client_spec) const;

    HsmObjectStoreClientPlugin::Ptr get_hsm_client_from_plugin(
        const ObjectStoreBackend& client_spec) const;

  private:
    ObjectStorePluginHandler::Ptr m_plugin_handler;
};

}  // namespace hestia