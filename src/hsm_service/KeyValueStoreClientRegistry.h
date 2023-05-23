#pragma once

#include "FileKeyValueStoreClient.h"
#include "Metadata.h"

#ifdef HAS_KVSAL
#include "KVSalKeyValueStore.h"
#endif

#include <string>

namespace hestia {
class KeyValueStoreClientSpec {
  public:
    enum class Type { FILE, KVSAL, REDIS };

    KeyValueStoreClientSpec(Type type, const Metadata& config = {}) :
        m_type(type), m_config(config)
    {
    }

    Type m_type{Type::FILE};
    Metadata m_config;
};


class KeyValueStoreClientRegistry {

  public:
    ~KeyValueStoreClientRegistry();
    static std::string to_string(const KeyValueStoreClientSpec& client_spec);

    static bool is_client_type_available(
        const KeyValueStoreClientSpec& client_spec);

    static std::unique_ptr<KeyValueStoreClient> get_client(
        const KeyValueStoreClientSpec& client_spec);
};
}  // namespace hestia