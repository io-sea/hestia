#pragma once

#include "KeyValueStoreClient.h"

#include <memory>
#include <string>

namespace hestia {

class RedisReplyWrapper;
class RedisContextWrapper;

class RedisKeyValueStoreClientConfig {
  public:
    std::string m_redis_backend_address{"127.0.0.1"};
    int m_redis_backend_port{6379};
};

class RedisKeyValueStoreClient : public KeyValueStoreClient {
  public:
    RedisKeyValueStoreClient();

    virtual ~RedisKeyValueStoreClient();

    void initialize(const Metadata& config) override;

    void do_initialize(const RedisKeyValueStoreClientConfig& config);

    bool string_exists(const std::string& key) const override;

    void string_get(const std::string& key, std::string& value) const override;

    void string_set(
        const std::string& key, const std::string& value) const override;

    void string_remove(const std::string& key) const override;

    void set_add(
        const std::string& key, const std::string& value) const override;

    void set_list(
        const std::string& key, std::vector<std::string>& value) const override;

    void set_remove(
        const std::string& key, const std::string& value) const override;

  private:
    std::unique_ptr<RedisReplyWrapper> make_request(
        const std::string& request) const;

    RedisKeyValueStoreClientConfig m_config;
    std::unique_ptr<RedisContextWrapper> m_context;
};
}  // namespace hestia