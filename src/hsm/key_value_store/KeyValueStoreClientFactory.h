#pragma once

#include "EnumUtils.h"
#include "Map.h"
#include "SerializeableWithFields.h"

#include "KeyValueStoreClient.h"

namespace hestia {
class KeyValueStoreClientConfig : public SerializeableWithFields {
  public:
    STRINGABLE_ENUM(Type, FILE, MEMORY, KVSAL, REDIS)

    KeyValueStoreClientConfig();

    KeyValueStoreClientConfig(const KeyValueStoreClientConfig& other);

    static std::string get_type();

    const Dictionary& get_config() const;

    Type get_client_type() const;

    std::string get_client_type_as_client() const;

    void set_client_type(Type client_type)
    {
        m_client_type.update_value(client_type);
    }

    KeyValueStoreClientConfig& operator=(
        const KeyValueStoreClientConfig& other);

  private:
    void init();

    static constexpr const char s_type[]{"key_value_store_client"};
    EnumField<Type, Type_enum_string_converter> m_client_type{
        "client_type", Type::FILE};
    RawDictField m_config{"config"};
};


class KeyValueStoreClientFactory {
  public:
    ~KeyValueStoreClientFactory();

    static std::unique_ptr<KeyValueStoreClient> get_client(
        const std::string& config_path,
        const KeyValueStoreClientConfig& client_config);
};
}  // namespace hestia