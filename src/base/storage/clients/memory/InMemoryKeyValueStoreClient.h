#pragma once

#include "KeyValueStoreClient.h"

#include <map>
#include <set>

namespace hestia {

class InMemoryKeyValueStoreClient : public KeyValueStoreClient {
  public:
    InMemoryKeyValueStoreClient();

    virtual ~InMemoryKeyValueStoreClient() = default;

    void initialize(const Metadata& config) override;

    std::string dump() const;

  private:
    bool string_exists(const std::string& key) const override;

    void string_get(const std::string& key, std::string& value) const override;

    void string_multi_get(
        const std::vector<std::string>& key,
        std::vector<std::string>& value) const override;

    void string_set(
        const std::string& key, const std::string& value) const override;

    void string_remove(const std::string& key) const override;

    void set_add(const std::string& key, const Uuid& value) const override;

    void set_list(
        const std::string& key, std::vector<Uuid>& value) const override;

    void set_remove(const std::string& key, const Uuid& value) const override;

    mutable std::unordered_map<std::string, std::string> m_string_db;
    mutable std::unordered_map<std::string, std::set<Uuid>> m_set_db;
};
}  // namespace hestia