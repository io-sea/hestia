#pragma once

#include "FileKeyValueStoreClient.h"
#include "KeyValueStore.h"

#ifdef HAS_KVSAL
#include "KVSalKeyValueStore.h"
#endif

#include <string>

enum class KeyValueStoreType { FILE, KVSAL };

namespace KeyValueStoreRegistry {

static std::string toString(KeyValueStoreType storeType)
{
    switch (storeType) {
        case KeyValueStoreType::FILE:
            return "FILE";
        case KeyValueStoreType::KVSAL:
            return "KVSAL";
        default:
            return "UNKNOWN";
    }
};

static bool isStoreTypeAvailable(KeyValueStoreType storeType)
{
    switch (storeType) {
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

static std::unique_ptr<KeyValueStore> getStore(KeyValueStoreType storeType)
{
    switch (storeType) {
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

}  // namespace KeyValueStoreRegistry