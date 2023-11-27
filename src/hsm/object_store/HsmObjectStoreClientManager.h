#pragma once

#include "HsmObjectStoreClientFactory.h"
#include "StorageTier.h"

#include <unordered_map>

namespace hestia {
class S3Client;

class HsmObjectStoreClientManager {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientManager>;

    HsmObjectStoreClientManager(
        HsmObjectStoreClientFactory::Ptr client_factory);

    ObjectStoreClient* get_client(ObjectStoreBackend::Type identifier) const;

    ObjectStoreClient* get_client(const std::string& tier_id) const;

    HsmObjectStoreClient* get_hsm_client(
        ObjectStoreBackend::Type identifier) const;

    HsmObjectStoreClient* get_hsm_client(const std::string& tier_id) const;

    ObjectStoreBackend::Type get_backend(const std::string& tier_id) const;

    bool has_client(const std::string& tier_id) const;

    bool have_same_client_types(
        const std::string& tier_id0, const std::string& tier_id1) const;

    bool is_hsm_client(const std::string& tier_id) const;

    void setup_clients(
        const std::string& cache_path,
        const std::string& node_id,
        S3Client* s3_client,
        const std::vector<StorageTier>& tiers,
        const std::vector<ObjectStoreBackend>& backends = {});

  private:
    bool has_backend(ObjectStoreBackend::Type backend) const;

    HsmObjectStoreClientFactory::Ptr m_client_factory;

    std::unordered_map<std::string, ObjectStoreBackend::Type> m_tier_backends;
    std::vector<ObjectStoreBackend::Type> m_backends;

    std::unordered_map<ObjectStoreBackend::Type, ObjectStoreClient::Ptr>
        m_clients;
    std::unordered_map<ObjectStoreBackend::Type, ObjectStoreClientPlugin::Ptr>
        m_plugin_clients;

    std::unordered_map<ObjectStoreBackend::Type, HsmObjectStoreClient::Ptr>
        m_hsm_clients;
    std::
        unordered_map<ObjectStoreBackend::Type, HsmObjectStoreClientPlugin::Ptr>
            m_hsm_plugin_clients;
};
}  // namespace hestia