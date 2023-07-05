#include "RedisKeyValueStoreClient.h"

#include "Logger.h"
#include "UuidUtils.h"

#include <hiredis.h>

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

    std::string as_string_or_nill()
    {
        if (m_reply->type == REDIS_REPLY_STRING) {
            return std::string(m_reply->str, m_reply->len);
        }
        else if (m_reply->type != REDIS_REPLY_NIL) {
            LOG_ERROR("Error making GET request");
            throw std::runtime_error("Error making GET request");
        }
        return {};
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

    void as_array(std::vector<Uuid>& array)
    {
        if (m_reply->type != REDIS_REPLY_ARRAY) {
            LOG_ERROR("Error making SADD request");
            throw std::runtime_error("Error making SADD request");
        }

        for (std::size_t idx = 0; idx < m_reply->elements; idx++) {
            auto each_reply = m_reply->element[idx];
            if (each_reply->type == REDIS_REPLY_STRING) {
                array.push_back(
                    UuidUtils::from_string({each_reply->str, each_reply->len}));
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

void RedisKeyValueStoreClient::initialize(const Metadata& config_data)
{
    RedisKeyValueStoreClientConfig config;
    auto on_item = [&config](const std::string& key, const std::string& value) {
        if (key == "backend_address") {
            config.m_redis_backend_address = value;
        }
        else if (key == "backend_port") {
            config.m_redis_backend_port = std::stoi(value);
        }
    };
    config_data.for_each_item(on_item);
    do_initialize(config);
}

void RedisKeyValueStoreClient::do_initialize(
    const RedisKeyValueStoreClientConfig& config)
{
    m_config     = config;
    auto context = redisConnect(
        m_config.m_redis_backend_address.c_str(),
        m_config.m_redis_backend_port);
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

bool RedisKeyValueStoreClient::string_exists(const std::string& key) const
{
    if (!key.empty()) {
        const std::string command = "EXISTS " + key;
        auto reply                = make_request(command);
        return reply->as_int() == 1;
    }
    return false;
}

void RedisKeyValueStoreClient::string_get(
    const std::string& key, std::string& value) const
{
    if (!key.empty()) {
        const std::string command = "GET " + key;
        auto reply                = make_request(command);
        value                     = reply->as_string_or_nill();
    }
}

void RedisKeyValueStoreClient::string_multi_get(
    const std::vector<std::string>& key, std::vector<std::string>& value) const
{
    if (!key.empty()) {
        std::string command = "MGET ";
        for (const auto& k : key) {
            command += k + " ";
        }
        auto reply = make_request(command);
        reply->as_array(value);
    }
}

void RedisKeyValueStoreClient::string_set(
    const std::string& key, const std::string& value) const
{
    const std::string command = "SET " + key + " %b";
    auto reply                = make_request(command, value);
    reply->check_ok();
}

void RedisKeyValueStoreClient::string_remove(const std::string& key) const
{
    const std::string command = "DEL " + key;
    auto reply                = make_request(command);
    reply->as_int();
}

void RedisKeyValueStoreClient::set_add(
    const std::string& key, const Uuid& value) const
{
    const std::string command =
        "SADD " + key + " " + UuidUtils::to_string(value);
    auto reply = make_request(command);
    reply->as_int();
}

void RedisKeyValueStoreClient::set_list(
    const std::string& key, std::vector<Uuid>& value) const
{
    if (!key.empty()) {
        const std::string command = "SMEMBERS " + key;
        auto reply                = make_request(command);
        reply->as_array(value);
    }
}

void RedisKeyValueStoreClient::set_remove(
    const std::string& key, const Uuid& value) const
{
    const std::string command =
        "SREM " + key + " " + UuidUtils::to_string(value);
    auto reply = make_request(command);
    reply->as_int();
}

}  // namespace hestia