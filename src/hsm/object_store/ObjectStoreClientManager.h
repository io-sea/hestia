#pragma once

#include "ObjectStoreBackendWithClient.h"
#include "ObjectStoreClientFactory.h"
#include "StorageTier.h"

#include <optional>
#include <unordered_map>

namespace hestia {
class S3Client;

struct ObjectStoreClientsConfig {
    std::string m_cache_path;
    std::string m_node_id;
    S3Client* m_s3_client{nullptr};
    std::vector<StorageTier> m_tiers;
};

class ObjectStoreClientManager {
  public:
    using Ptr = std::unique_ptr<ObjectStoreClientManager>;

    ObjectStoreClientManager(ObjectStoreClientFactory::Ptr client_factory);

    ObjectStoreBackendWithClient* get_backend(const std::string& tier_id) const;

    bool has_backend(const std::string& tier_id) const;

    bool have_same_backend(
        const std::string& tier_id0, const std::string& tier_id1) const;

    void setup_clients(const ObjectStoreClientsConfig& config);

  private:
    ObjectStoreClientFactory::Ptr m_client_factory;

  protected:
    std::vector<ObjectStoreBackendWithClient::Ptr> m_backends;

  private:
    void initialize_backends(const ObjectStoreClientsConfig& config);

    void check_if_client_available(const ObjectStoreBackend& backend) const;

    void setup_built_in_client(
        ObjectStoreBackendWithClient* backend,
        const ObjectStoreClientsConfig& config);

    void setup_built_in_hsm_client(
        ObjectStoreBackendWithClient* backend,
        const ObjectStoreClientsConfig& config);

    void setup_plugin_client(
        ObjectStoreBackendWithClient* backend,
        const ObjectStoreClientsConfig& config);

    void setup_plugin_hsm_client(
        ObjectStoreBackendWithClient* backend,
        const ObjectStoreClientsConfig& config);

    std::optional<std::size_t> get_backend_offset(
        const std::string& tier_id) const;

    // Values here are offsets into the m_backends vector. There is a
    // many-to-many relationship. A tier can have multiple backends and a
    // backend can be on multiple tiers.
    std::unordered_map<std::string, std::vector<std::size_t>>
        m_tier_backend_offsets;
};
}  // namespace hestia