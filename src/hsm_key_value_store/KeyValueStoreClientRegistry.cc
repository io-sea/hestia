#include "KeyValueStoreClientRegistry.h"

#include "RedisKeyValueStoreClient.h"

#include "HsmKeyValueStore.h"
#include "Logger.h"

namespace hestia {

KeyValueStoreClientRegistry::~KeyValueStoreClientRegistry() {}

std::string KeyValueStoreClientRegistry::to_string(
    const KeyValueStoreClientSpec& client_spec)
{
    switch (client_spec.m_type) {
        case KeyValueStoreClientSpec::Type::FILE:
            return "FILE";
        case KeyValueStoreClientSpec::Type::KVSAL:
            return "KVSAL";
        case KeyValueStoreClientSpec::Type::REDIS:
            return "REDIS";
        default:
            return "UNKNOWN";
    }
}

bool KeyValueStoreClientRegistry::is_store_type_available(
    const KeyValueStoreClientSpec& client_spec)
{
    switch (client_spec.m_type) {
        case KeyValueStoreClientSpec::Type::FILE:
            return true;
        case KeyValueStoreClientSpec::Type::KVSAL:
#ifdef HAS_KVSAL
            return true;
#else
            return false;
#endif
        case KeyValueStoreClientSpec::Type::REDIS:
            return true;
        default:
            return false;
    }
}

std::unique_ptr<HsmKeyValueStore> KeyValueStoreClientRegistry::get_store(
    const KeyValueStoreClientSpec& client_spec)
{
    switch (client_spec.m_type) {
        case KeyValueStoreClientSpec::Type::FILE: {
            LOG_INFO("Using file key-value store client");
            auto client = std::make_unique<FileKeyValueStoreClient>();
            client->initialize(client_spec.m_config);
            return std::make_unique<HsmKeyValueStore>(std::move(client));
        }
        case KeyValueStoreClientSpec::Type::KVSAL:
#ifdef HAS_KVSAL
            return std::make_unique<KvsalKeyValueStore>();
#else
            return nullptr;
#endif
        case KeyValueStoreClientSpec::Type::REDIS: {
            LOG_INFO("Using redis key-value store client");
            auto client = std::make_unique<RedisKeyValueStoreClient>();
            client->initialize(client_spec.m_config);
            return std::make_unique<HsmKeyValueStore>(std::move(client));
        }
        default: {
            LOG_INFO(
                "Unknown kv-store type - defaulting to file key-value store client");
            auto client = std::make_unique<FileKeyValueStoreClient>();
            return std::make_unique<HsmKeyValueStore>(std::move(client));
        }
    }
}

}  // namespace hestia