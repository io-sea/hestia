#pragma once

#include "FileKeyValueStoreClient.h"
#include "KeyValueStore.h"

#ifdef HAS_KVSAL
#include "KVSalKeyValueStore.h"
#endif

#include <string>

namespace hestia {
enum class KeyValueStoreType { FILE, KVSAL };

class KeyValueStoreClientRegistry {

  public:
    static std::string to_string(KeyValueStoreType store_type);

    static bool is_store_type_available(KeyValueStoreType store_type);

    static std::unique_ptr<KeyValueStore> get_store(
        KeyValueStoreType store_type);
};
}  // namespace hestia