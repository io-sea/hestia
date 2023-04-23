#pragma once

#include "FileKeyValueStoreClient.h"
#include "KeyValueStore.h"

#ifdef HAS_KVSAL
#include "KVSalKeyValueStore.h"
#endif

#include <string>

enum class KeyValueStoreType { FILE, KVSAL };

namespace key_value_store_registry {

static std::string to_string(KeyValueStoreType store_type)
{
    switch (store_type) {
        case KeyValueStoreType::FILE:
            return "FILE";
        case KeyValueStoreType::KVSAL:
            return "KVSAL";
        default:
            return "UNKNOWN";
    }
};

static bool is_store_type_available(KeyValueStoreType store_type)
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
};

static std::unique_ptr<KeyValueStore> get_store(KeyValueStoreType store_type)
{
    switch (store_type) {
        case KeyValueStoreType::FILE:
            // auto client = std::make_unique<FileKeyValueStoreClient>();
            return std::make_unique<KeyValueStore>(nullptr);
        case KeyValueStoreType::KVSAL:
#ifdef HAS_KVSAL
            return std::make_unique<KvsalKeyValueStore>();
#else
            return nullptr;
#endif
        default:
            return nullptr;
    }
};

}  // namespace key_value_store_registry