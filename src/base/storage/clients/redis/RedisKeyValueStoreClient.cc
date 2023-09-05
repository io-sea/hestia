#include "RedisKeyValueStoreClient.h"

#include "Logger.h"

#include <hiredis.h>

#include <optional>
#include <stdexcept>

namespace hestia {

class RedisReplyWrapper {
  public:
    RedisReplyWrapper(redisReply* reply) : m_reply(reply) {}

    ~RedisReplyWrapper() { freeReplyObject(m_reply); }

    int as_int()
    {
        if (m_reply->type == REDIS_REPLY_INTEGER) {
            return m_reply->integer;
        }
        else {
            const std::string msg = "Unexpected reply type - should be int";
            LOG_ERROR(msg);
            throw std::runtime_error(msg);
        }
    }

    std::optional<std::string> as_string_or_nill()
    {
        if (m_reply->type == REDIS_REPLY_STRING) {
            return std::string(m_reply->str, m_reply->len);
        }
        else if (m_reply->type == REDIS_REPLY_NIL) {
            return std::nullopt;
        }
        else {
            LOG_ERROR(
                "Error making GET request, response code: " << m_reply->type);
            throw std::runtime_error("Error making GET request");
        }
    }

    void as_array(std::vector<std::string>& array)
    {
        if (m_reply->type != REDIS_REPLY_ARRAY) {
            LOG_ERROR("Error making SADD request");
            throw std::runtime_error("Error making SADD request");
        }

        for (std::size_t idx = 0; idx < m_reply->elements; idx++) {
            auto each_reply = m_reply->element[idx];
            if (each_reply->type == REDIS_REPLY_STRING) {
                array.push_back(std::string(each_reply->str, each_reply->len));
            }
            else if (m_reply->type != REDIS_REPLY_NIL) {
                LOG_ERROR("Error making MULTIGET request");
                throw std::runtime_error("Error making MULTIGET request");
            }
        }
    }

    void check_ok()
    {
        if (!(m_reply->type == REDIS_REPLY_STATUS
              && std::string(m_reply->str, m_reply->len) == "OK")) {
            LOG_ERROR("Error making SET request");
            throw std::runtime_error("Error making SET request");
        }
    }

    redisReply* m_reply{nullptr};
};

class RedisContextWrapper {
  public:
    RedisContextWrapper(redisContext* context) : m_context(context) {}

    ~RedisContextWrapper() { redisFree(m_context); }

    void check_if_valid()
    {
        if (m_context == nullptr || m_context->err != 0) {
            if (m_context != nullptr) {
                const std::string msg = "Error connecting to redis: "
                                        + std::string(m_context->errstr);
                LOG_ERROR(msg);
                throw std::runtime_error(msg);
            }
            else {
                const std::string msg = "Can't allocate redis context";
                LOG_ERROR("Can't allocate redis context.");
                throw std::runtime_error(msg);
            }
        }
    }

    redisContext* m_context{nullptr};
};

RedisKeyValueStoreClient::RedisKeyValueStoreClient() : KeyValueStoreClient() {}

RedisKeyValueStoreClient::~RedisKeyValueStoreClient()
{
    LOG_INFO("Destroying");
}

void RedisKeyValueStoreClient::initialize(
    const std::string& cache_path, const Dictionary& config_data)
{
    RedisKeyValueStoreClientConfig config;
    config.deserialize(config_data);
    do_initialize(cache_path, config);
}

void RedisKeyValueStoreClient::do_initialize(
    const std::string&, const RedisKeyValueStoreClientConfig& config)
{
    m_config     = config;
    auto context = redisConnect(
        m_config.m_backend_address.get_value().c_str(),
        m_config.m_backend_port.get_value());
    m_context = std::make_unique<RedisContextWrapper>(context);
    m_context->check_if_valid();
}

std::unique_ptr<RedisReplyWrapper> RedisKeyValueStoreClient::make_request(
    const std::string& request, const std::string& binary_string) const
{
    redisReply* reply;
    if (binary_string.empty()) {
        reply = reinterpret_cast<redisReply*>(
            redisCommand(m_context->m_context, request.c_str()));
    }
    else {
        reply = reinterpret_cast<redisReply*>(redisCommand(
            m_context->m_context, request.c_str(), binary_string.c_str(),
            binary_string.length()));
    }
    if (reply == nullptr) {
        m_context->check_if_valid();
    }
    return std::make_unique<RedisReplyWrapper>(reply);
}

void RedisKeyValueStoreClient::string_exists(
    const std::vector<std::string>& keys, std::vector<bool>& found) const
{
    for (const auto& key : keys) {
        if (!key.empty()) {
            const std::string command = "EXISTS " + key;
            auto reply                = make_request(command);
            found.push_back(reply->as_int() == 1);
        }
        else {
            found.push_back(false);
        }
    }
}

void RedisKeyValueStoreClient::string_get(
    const std::vector<std::string>& keys,
    std::vector<std::string>& values) const
{
    if (keys.size() == 1) {
        const std::string command = "GET " + keys[0];
        auto reply                = make_request(command);
        const auto value_opt      = reply->as_string_or_nill();
        if (value_opt) {
            values.push_back(*value_opt);
        }
        else {
            values.push_back("");
        }
    }
    else if (!keys.empty()) {
        std::string command = "MGET ";
        for (const auto& k : keys) {
            command += k + " ";
        }
        auto reply = make_request(command);
        reply->as_array(values);
    }
}

void RedisKeyValueStoreClient::string_set(
    const std::vector<KeyValuePair>& kv_pairs) const
{
    for (const auto& pair : kv_pairs) {
        const std::string command = "SET " + pair.first + " %b";
        auto reply                = make_request(command, pair.second);
        reply->check_ok();
    }
}

void RedisKeyValueStoreClient::string_remove(
    const std::vector<std::string>& keys) const
{
    for (const auto& key : keys) {
        const std::string command = "DEL " + key;
        auto reply                = make_request(command);
        reply->as_int();
    }
}

void RedisKeyValueStoreClient::set_add(const VecKeyValuePair& entries) const
{
    for (const auto& entry : entries) {
        const std::string command = "SADD " + entry.first + " " + entry.second;
        auto reply                = make_request(command);
        reply->as_int();
    }
}

void RedisKeyValueStoreClient::set_list(
    const std::vector<std::string>& keys,
    std::vector<std::vector<std::string>>& total_values) const
{
    for (const auto& key : keys) {
        std::vector<std::string> value;
        if (!key.empty()) {
            const std::string command = "SMEMBERS " + key;
            auto reply                = make_request(command);
            reply->as_array(value);
        }
        total_values.push_back(value);
    }
}

void RedisKeyValueStoreClient::set_remove(const VecKeyValuePair& entries) const
{
    for (const auto& entry : entries) {
        const std::string command = "SREM " + entry.first + " " + entry.second;
        auto reply                = make_request(command);
        reply->as_int();
    }
}

}  // namespace hestia