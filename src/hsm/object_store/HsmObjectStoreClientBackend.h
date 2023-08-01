#pragma once

#include "EnumUtils.h"
#include "SerializeableWithFields.h"

namespace hestia {
class HsmObjectStoreClientBackend : public SerializeableWithFields {
  public:
    STRINGABLE_ENUM(
        Type,
        FILE,
        MEMORY,
        PHOBOS,
        MOTR,
        S3,
        FILE_HSM,
        MEMORY_HSM,
        MOCK_PHOBOS,
        MOCK_MOTR,
        MOCK_S3)

    HsmObjectStoreClientBackend();

    HsmObjectStoreClientBackend(
        Type client_type, const std::string& identifier);

    HsmObjectStoreClientBackend(const HsmObjectStoreClientBackend& other);

    const Dictionary& get_config() const;

    const std::string& get_identifier() const;

    static std::string get_type();

    bool is_hsm() const;

    bool is_plugin() const;

    bool is_mock() const;

    bool is_built_in() const;

    void set_identifier(const std::string& identifier)
    {
        m_identifier.update_value(identifier);
    }

    std::string get_plugin_path() const { return m_plugin_path.get_value(); }

    std::string to_string() const;

    void set_config(const Dictionary& dict) { m_config.update_value(dict); }

    HsmObjectStoreClientBackend& operator=(
        const HsmObjectStoreClientBackend& other);

  private:
    void init();

    static constexpr const char s_type[]{"object_store_client"};
    EnumField<Type, Type_enum_string_converter> m_backend_type{"backend_type"};
    StringField m_identifier{"identifier"};
    StringField m_plugin_path{"plugin_path"};
    RawDictField m_config{"config"};
};

}  // namespace hestia