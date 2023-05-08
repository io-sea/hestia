#include "KeyValueStoreClientRegistry.h"

#include "KeyValueStore.h"
#include "Logger.h"

namespace hestia {

std::string KeyValueStoreClientRegistry::to_string(KeyValueStoreType store_type)
{
    switch (store_type) {
        case KeyValueStoreType::FILE:
            return "FILE";
        case KeyValueStoreType::KVSAL:
            return "KVSAL";
        default:
            return "UNKNOWN";
    }
}

bool KeyValueStoreClientRegistry::is_store_type_available(
    KeyValueStoreType store_type)
{
    switch (store_type) {
        case KeyValueStoreType::FILE:
            return true;
        case KeyValueStoreType::KVSAL:
#ifdef HAS_KVSAL
            return true;
#else
            return false;
#endif
        default:
            return false;
    }
}

std::unique_ptr<KeyValueStore> KeyValueStoreClientRegistry::get_store(
    KeyValueStoreType store_type)
{
    switch (store_type) {
        case KeyValueStoreType::FILE: {
            LOG_INFO("Using file key-value store client");
            auto client = std::make_unique<FileKeyValueStoreClient>();
            return std::make_unique<KeyValueStore>(std::move(client));
        }
        case KeyValueStoreType::KVSAL:
#ifdef HAS_KVSAL
            return std::make_unique<KvsalKeyValueStore>();
#else
            return nullptr;
#endif
        default: {
            LOG_INFO(
                "Unknown kv-store type - defaulting to file key-value store client");
            auto client = std::make_unique<FileKeyValueStoreClient>();
            return std::make_unique<KeyValueStore>(std::move(client));
        }
    }
}

}  // namespace hestia