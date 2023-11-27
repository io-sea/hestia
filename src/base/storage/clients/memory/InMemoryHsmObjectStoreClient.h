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

  private:
    void get(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void put(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void remove(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void make_object_store_request(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func,
        ObjectStoreRequestMethod method,
        Stream* stream = nullptr) const;

    void copy(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void move(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    InMemoryObjectStoreClient* get_tier_client(
        const std::string& tier_id) const;

    std::unordered_map<std::string, std::unique_ptr<InMemoryObjectStoreClient>>
        m_tiers;
};
}  // namespace hestia