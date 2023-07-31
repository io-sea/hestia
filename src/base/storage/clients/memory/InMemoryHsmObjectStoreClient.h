#pragma once

#include "HsmObjectStoreClient.h"
#include "InMemoryObjectStoreClient.h"
#include "SerializeableWithFields.h"

#include <unordered_map>

namespace hestia {
class InMemoryObjectStoreClientConfig : public SerializeableWithFields {
  public:
    InMemoryObjectStoreClientConfig() :
        SerializeableWithFields("in_memory_hsm_object_store_client")
    {
        register_sequence_field(&m_tier_ids);
    }

    void set_tiers(const std::vector<std::string>& tiers)
    {
        m_tier_ids.get_container_as_writeable() = tiers;
    }

    ScalarSequenceField<std::vector<std::string>> m_tier_ids{"tiers"};
};

class InMemoryHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<InMemoryHsmObjectStoreClient>;

    InMemoryHsmObjectStoreClient();

    ~InMemoryHsmObjectStoreClient();

    static Ptr create();

    static std::string get_registry_identifier();

    void initialize(
        const std::string& cache_path, const Dictionary& config) override;

    void do_initialize(
        const std::string& cache_path,
        const InMemoryObjectStoreClientConfig& config);

    std::string dump() const;

  private:
    void put(
        const HsmObjectStoreRequest& request, Stream* stream) const override;

    void get(
        const HsmObjectStoreRequest& request,
        StorageObject& object,
        Stream* stream) const override;

    void remove(const HsmObjectStoreRequest& request) const override;

    void copy(const HsmObjectStoreRequest& request) const override;

    void move(const HsmObjectStoreRequest& request) const override;

    InMemoryObjectStoreClient* get_tier_client(uint8_t tier) const;

    std::unordered_map<std::string, std::unique_ptr<InMemoryObjectStoreClient>>
        m_tiers;
};
}  // namespace hestia