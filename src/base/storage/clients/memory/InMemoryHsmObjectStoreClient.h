#pragma once

#include "HsmObjectStoreClient.h"
#include "InMemoryObjectStoreClient.h"
#include "SerializeableWithFields.h"

#include <unordered_map>

namespace hestia {
class InMemoryObjectStoreClientConfig : public SerializeableWithFields {
  public:
    InMemoryObjectStoreClientConfig();

    InMemoryObjectStoreClientConfig(
        const InMemoryObjectStoreClientConfig& other);

    InMemoryObjectStoreClientConfig& operator=(
        const InMemoryObjectStoreClientConfig& other);

  private:
    void init();
};

class InMemoryHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<InMemoryHsmObjectStoreClient>;

    InMemoryHsmObjectStoreClient();

    ~InMemoryHsmObjectStoreClient();

    static Ptr create();

    static std::string get_registry_identifier();

    void initialize(
        const std::string& id,
        const std::string& cache_path,
        const Dictionary& config) override;

    void do_initialize(
        const std::string& id,
        const std::string& cache_path,
        const InMemoryObjectStoreClientConfig& config);

    std::string dump() const;

  protected:
    void get(HsmObjectStoreContext& ctx) const override;

  private:
    void put(HsmObjectStoreContext& ctx) const override;

    void remove(HsmObjectStoreContext& ctx) const override;

    void copy(HsmObjectStoreContext& ctx) const override;

    void move(HsmObjectStoreContext& ctx) const override;

    void make_object_store_request(
        HsmObjectStoreContext& ctx, ObjectStoreRequestMethod method) const;

    InMemoryObjectStoreClient* get_tier_client(
        const std::string& tier_id) const;

    std::unordered_map<std::string, InMemoryObjectStoreClient::Ptr> m_tiers;
};
}  // namespace hestia