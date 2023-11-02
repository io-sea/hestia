#pragma once

#include "CrudRequest.h"
#include "CrudResponse.h"
#include "Dictionary.h"

#include "KeyValueFieldContext.h"

#include <functional>

namespace hestia {
class KeyValueReadContext : public KeyValueFieldContext {
  public:
    using dbGetItemFunc      = std::function<std::string(const std::string&)>;
    using dbGetSetsFunc      = std::function<void(
        const std::vector<std::string>&,
        std::vector<std::vector<std::string>>&)>;
    using idFromParentIdFunc = std::function<std::string(
        const std::string&,
        const std::string&,
        const std::string&,
        const CrudUserContext&)>;

    KeyValueReadContext(
        const CrudSerializer* serializer,
        const std::string& key_prefix,
        dbGetItemFunc db_get_item_func,
        dbGetSetsFunc db_get_sets_func,
        idFromParentIdFunc id_from_parent_id_func);

    bool serialize_request(const CrudRequest& request);

    const std::vector<std::string>& get_index_keys() const;

    const std::vector<std::string>& get_foreign_key_proxy_keys() const;

    const std::vector<std::string>& get_one_to_one_keys() const;

    void get_foreign_key_query(
        const std::vector<std::vector<std::string>>& ids,
        std::vector<std::string>& keys,
        std::vector<std::size_t>& sizes) const;

    void get_one_to_one_key_query(
        const std::vector<std::vector<std::string>>& ids,
        std::vector<std::string>& keys,
        std::vector<std::size_t>& sizes) const;

    bool has_foreign_key_content() const;

    bool has_one_to_one_content() const;

    void process_foreign_key_content(
        const std::vector<std::string>& db_values,
        const std::vector<std::size_t>& sizes,
        Dictionary& foreign_key_dicts) const;

    void process_one_to_one_content(
        const std::vector<std::string>& db_values, Dictionary& key_dicts) const;

    void merge_proxy_content(
        const Dictionary& foreign_key_dict, Dictionary& read_result) const;

  private:
    void add_item_id(const std::string& item_id);

    void add_db_item_to_dict(
        const std::string& db_item, Dictionary& dict) const;

    void add_db_items_to_dict(
        const std::vector<std::string>& db_items,
        const std::string& name,
        std::size_t offset,
        std::size_t size,
        Dictionary& dict) const;

    std::string get_proxy_key(
        const std::string& type, const std::string& id) const;

    std::string get_id_from_name(const CrudIdentifier& id) const;

    bool serialize_ids(
        const CrudIdentifierCollection& ids,
        const CrudUserContext& user_context);

    bool serialize_filter(const CrudQuery& query);

    void serialize_empty();

    void update_foreign_proxy_keys(const std::string& item_id);

    void update_one_to_one_keys(const std::string& item_id);

    std::vector<std::string> m_index_keys;

    std::vector<std::string> m_foreign_key_proxy_keys;
    VecKeyValuePair m_foreign_key_proxies;

    std::vector<std::string> m_one_to_one_keys;
    VecKeyValuePair m_one_to_one_key_proxies;

    dbGetItemFunc m_db_get_item_func;
    dbGetSetsFunc m_db_get_sets_func;
    idFromParentIdFunc m_id_from_parent_id_func;
};
}  // namespace hestia