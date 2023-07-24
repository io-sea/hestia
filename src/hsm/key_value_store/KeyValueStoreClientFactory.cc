#include "KeyValueStoreClientFactory.h"

#include "RedisKeyValueStoreClient.h"
#ifdef HAS_KVSAL
#include "KVSalKeyValueStore.h"
#endif
#include "FileKeyValueStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"

#include "Logger.h"

namespace hestia {

KeyValueStoreClientConfig::KeyValueStoreClientConfig() :
    SerializeableWithFields(s_type)
{
    init();
}

KeyValueStoreClientConfig::KeyValueStoreClientConfig(
    const KeyValueStoreClientConfig& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

KeyValueStoreClientConfig& KeyValueStoreClientConfig::operator=(
    const KeyValueStoreClientConfig& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_client_type = other.m_client_type;
        m_config      = other.m_config;
        init();
    }
    return *this;
}

void KeyValueStoreClientConfig::init()
{
    register_scalar_field(&m_client_type);
    register_map_field(&m_config);
}

std::string KeyValueStoreClientConfig::get_type()
{
    return s_type;
}

const Dictionary& KeyValueStoreClientConfig::get_config() const
{
    return m_config.value();
}

std::string KeyValueStoreClientConfig::get_client_type_as_client() const
{
    return Type_enum_string_converter().init().to_string(
        m_client_type.get_value());
}

KeyValueStoreClientConfig::Type KeyValueStoreClientConfig::get_client_type()
    const
{
    return m_client_type.get_value();
}

KeyValueStoreClientFactory::~KeyValueStoreClientFactory() {}

std::unique_ptr<KeyValueStoreClient> KeyValueStoreClientFactory::get_client(
    const std::string& config_path,
    const KeyValueStoreClientConfig& client_config)
{
    std::unique_ptr<KeyValueStoreClient> client;

    switch (client_config.get_client_type()) {
        case KeyValueStoreClientConfig::Type::FILE: {
            LOG_INFO("Using file key-value store client");
            client = std::make_unique<FileKeyValueStoreClient>();
            break;
        }
        case KeyValueStoreClientConfig::Type::KVSAL:
#ifdef HAS_KVSAL
            return std::make_unique<KvsalKeyValueStore>();
#else
            return nullptr;
#endif
        case KeyValueStoreClientConfig::Type::REDIS: {
            LOG_INFO("Using redis key-value store client");
            client = std::make_unique<RedisKeyValueStoreClient>();
            break;
        }
        case KeyValueStoreClientConfig::Type::MEMORY: {
            LOG_INFO("Using in memory key-value store client");
            client = std::make_unique<InMemoryKeyValueStoreClient>();
            break;
        }
        default: {
            LOG_INFO(
                "Unknown kv-store type - defaulting to file key-value store client");
            client = std::make_unique<FileKeyValueStoreClient>();
        }
    }

    if (client) {
        client->initialize(config_path, client_config.get_config());
    }
    return client;
}

}  // namespace hestia