#pragma once

#include "KeyValueStoreClient.h"
#include "SerializeableWithFields.h"

#include <memory>
#include <string>

namespace hestia {

class RedisReplyWrapper;
class RedisContextWrapper;

class RedisKeyValueStoreClientConfig : public SerializeableWithFields {
  public:
    RedisKeyValueStoreClientConfig() :
        SerializeableWithFields("redis_kv_store_config")
    {
        register_scalar_field(&m_backend_address);
        register_scalar_field(&m_backend_port);
    }

    StringField m_backend_address{"backend_address", "127.0.0.1"};
    UIntegerField m_backend_port{"backend_port", 6379};
};

class RedisKeyValueStoreClient : public KeyValueStoreClient {
  public:
    RedisKeyValueStoreClient();

    virtual ~RedisKeyValueStoreClient();

    void initialize(
        const std::string& cache_path, const Dictionary& config_data) override;

    void do_initialize(
        const std::string& cache_path,
        const RedisKeyValueStoreClientConfig& config);

    void string_exists(
        const std::vector<std::string>& key,
        std::vector<bool>& found) const override;

    void string_get(
        const std::vector<std::string>& key,
        std::vector<std::string>& value) const override;

    void string_set(const VecKeyValuePair& kv_pairs) const override;

    void string_remove(const std::vector<std::string>& key) const override;

    void set_add(const VecKeyValuePair& entry) const override;

    void set_list(
        const std::vector<std::string>& key,
        std::vector<std::vector<std::string>>& values) const override;

    void set_remove(const VecKeyValuePair& entry) const override;

  private:
    std::unique_ptr<RedisReplyWrapper> make_request(
        const std::string& request,
        const std::string& binary_string = "") const;

    RedisKeyValueStoreClientConfig m_config;
    std::unique_ptr<RedisContextWrapper> m_context;
};
}  // namespace hestia