#pragma once

#include "FileKeyValueStoreClient.h"
#include "HsmKeyValueStore.h"
#include "Metadata.h"

#ifdef HAS_KVSAL
#include "KVSalKeyValueStore.h"
#endif

#include <string>

namespace hestia {
class KeyValueStoreClientSpec {
  public:
    enum class Type { FILE, KVSAL };

    KeyValueStoreClientSpec(Type type, const Metadata& config = {}) :
        m_type(type), m_config(config)
    {
    }

    Type m_type{Type::FILE};
    Metadata m_config;
};


class KeyValueStoreClientRegistry {

  public:
    static std::string to_string(const KeyValueStoreClientSpec& client_spec);

    static bool is_store_type_available(
        const KeyValueStoreClientSpec& client_spec);

    static std::unique_ptr<HsmKeyValueStore> get_store(
        const KeyValueStoreClientSpec& client_spec);
};
}  // namespace hestia