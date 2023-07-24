#pragma once

#include "KeyValueStoreClient.h"

#include <map>
#include <set>

namespace hestia {

class InMemoryKeyValueStoreClient : public KeyValueStoreClient {
  public:
    InMemoryKeyValueStoreClient();

    virtual ~InMemoryKeyValueStoreClient() = default;

    void initialize(
        const std::string& cache_path, const Dictionary& config) override;

    std::string dump() const;

  private:
    void string_exists(
        const std::vector<std::string>& key,
        std::vector<bool>& found) const override;

    void string_get(
        const std::vector<std::string>& key,
        std::vector<std::string>& value) const override;

    void string_set(const VecKeyValuePair& kv_pair) const override;

    void string_remove(const std::vector<std::string>& key) const override;

    void set_add(const VecKeyValuePair& entry) const override;

    void set_list(
        const std::vector<std::string>& key,
        std::vector<std::vector<std::string>>& values) const override;

    void set_remove(const VecKeyValuePair& entry) const override;

    mutable std::unordered_map<std::string, std::string> m_string_db;
    mutable std::unordered_map<std::string, std::set<std::string>> m_set_db;
};
}  // namespace hestia