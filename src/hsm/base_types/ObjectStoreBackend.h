#pragma once

#include "EnumUtils.h"
#include "HsmItem.h"
#include "Model.h"

namespace hestia {
class ObjectStoreBackend : public HsmItem, public Model {
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
        MOCK_S3,
        CUSTOM,
        CUSTOM_HSM,
        UNKNOWN)

    ObjectStoreBackend();

    ObjectStoreBackend(Type client_type);

    ObjectStoreBackend(const ObjectStoreBackend& other);

    const Dictionary& get_config() const;

    Type get_backend() const;

    const std::vector<std::string> get_tier_ids() const;

    static std::string get_type();

    bool is_hsm() const;

    bool is_plugin() const;

    bool is_mock() const;

    bool is_built_in() const;

    std::string get_plugin_path() const { return m_plugin_path.get_value(); }

    const std::vector<std::string>& get_tier_names() const;

    std::string to_string() const;

    void set_config(const Dictionary& dict) { m_config.update_value(dict); }

    void set_node_id(const std::string& id);

    void set_tier_ids(const std::vector<std::string>& ids);

    void set_tier_names(const std::vector<std::string>& tier_names);

    void add_tier_id(const std::string& id);

    ObjectStoreBackend& operator=(const ObjectStoreBackend& other);

  private:
    void init();

    static constexpr const char s_type[]{"object_store_client"};
    EnumField<
        ObjectStoreBackend::Type,
        ObjectStoreBackend::Type_enum_string_converter>
        m_backend_type{"backend_type"};
    StringField m_custom_identifier{"custom_identifier"};
    StringField m_plugin_path{"plugin_path"};
    RawDictField m_config{"config"};
    ScalarSequenceField<std::vector<std::string>> m_tier_names{"tier_names"};

    ManyToManyField m_tiers{"tiers", HsmItem::tier_name};
    ForeignKeyField m_node{"node", HsmItem::hsm_node_name, true};
};

}  // namespace hestia