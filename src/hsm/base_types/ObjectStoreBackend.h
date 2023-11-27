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
        CUSTOM,
        CUSTOM_HSM,
        UNKNOWN)

    ObjectStoreBackend();

    ObjectStoreBackend(Type client_type);

    ObjectStoreBackend(const ObjectStoreBackend& other);

    void add_tier_id(const std::string& id);

    const Dictionary& get_config() const;

    Type get_backend() const;

    std::string get_backend_as_string() const;

    const std::vector<std::string> get_tier_ids() const;

    const std::string& get_node_id() const;

    static std::string get_type();

    bool has_tier(const std::string& tier_id) const;

    bool is_hsm() const;

    bool is_plugin() const;

    bool is_mock() const;

    bool is_built_in() const;

    std::string get_plugin_path() const;

    std::string to_string() const;

    void set_config(const Dictionary& dict) { m_config.update_value(dict); }

    void set_node_id(const std::string& id);

    void set_tier_ids(const std::vector<std::string>& ids);

    ObjectStoreBackend& operator=(const ObjectStoreBackend& other);

  private:
    void init();

    EnumField<
        ObjectStoreBackend::Type,
        ObjectStoreBackend::Type_enum_string_converter>
        m_backend_type{"backend_type"};
    StringField m_custom_identifier{"custom_identifier"};
    StringField m_plugin_path{"plugin_path"};
    RawDictField m_config{"config"};

    ManyToManyField m_tiers{"tiers", HsmItem::tier_name};
    ForeignKeyField m_node{"node", HsmItem::hsm_node_name, true};
};

}  // namespace hestia