#pragma once

#include "CrudClient.h"
#include "Response.h"

namespace hestia {

class KeyValueStoreClient;

class KeyValueCrudClient : public CrudClient {
  public:
    KeyValueCrudClient(
        const CrudClientConfig& config,
        AdapterCollectionPtr adapters,
        KeyValueStoreClient* client,
        IdGenerator* id_generator   = nullptr,
        TimeProvider* time_provider = nullptr);

    virtual ~KeyValueCrudClient();

  private:
    void create(
        const CrudRequest& request,
        CrudResponse& response,
        bool record_modified_attrs = false) override;

    void read(
        const CrudRequest& request, CrudResponse& response) const override;

    void update(
        const CrudRequest& request,
        CrudResponse& response,
        bool record_modified_attrs = false) const override;

    void remove(
        const CrudRequest& request, CrudResponse& response) const override;

    void identify(
        const CrudRequest& request, CrudResponse& response) const override;

    void lock(const CrudIdentifier& id, CrudLockType lock_type) const override;

    void unlock(
        const CrudIdentifier& id, CrudLockType lock_type) const override;

    bool is_locked(
        const CrudIdentifier& id, CrudLockType lock_type) const override;

    struct Fields {
        std::vector<SerializeableWithFields::VecIndexField> m_index;
        std::vector<std::string> m_parent_names;
        std::vector<std::string> m_parent_ids;
        std::vector<Model::VecForeignKeyContext> m_foreign_key;
        std::vector<VecKeyValuePair> m_foreign_key_id_replacements;
        std::vector<VecKeyValuePair> m_one_to_one;
        std::vector<std::vector<Model::TypeIdsPair>> m_many_many;
    };

    void process_fields(Model* item, Fields& fields) const;

    void process_items(
        const CrudRequest& request,
        std::vector<std::string>& ids,
        Fields& fields,
        Dictionary& content) const;

    void process_ids(
        const CrudRequest& request,
        std::vector<std::string>& ids,
        Fields& fields,
        const Dictionary& attributes,
        Dictionary& content) const;

    void process_update_ids(
        const CrudRequest& request, std::vector<std::string>& ids) const;

    void process_empty(
        std::vector<std::string>& ids,
        Fields& fields,
        const Dictionary& attributes,
        Dictionary& content) const;

    void prepare_create_keys(
        std::vector<KeyValuePair>& string_set_kv_pairs,
        std::vector<KeyValuePair>& set_add_kv_pairs,
        std::vector<std::string>& ids,
        const Fields& fields,
        const Dictionary& content,
        const Dictionary& create_context_dict,
        const std::string& primary_key_name) const;

    bool prepare_query_keys_with_id(
        std::vector<std::string>& string_get_keys,
        std::vector<std::string>& foreign_key_proxy_keys,
        const VecKeyValuePair& fields,
        const CrudQuery& query) const;

    bool prepare_query_keys_with_filter(
        std::vector<std::string>& string_get_keys,
        std::vector<std::string>& foreign_key_proxy_keys,
        const VecKeyValuePair& fields,
        const CrudQuery& query) const;

    void prepare_query_keys_empty(
        std::vector<std::string>& string_get_keys,
        std::vector<std::string>& foreign_key_proxy_keys,
        const VecKeyValuePair& fields) const;

    void update_foreign_proxy_keys(
        const std::string& id,
        const VecKeyValuePair& fields,
        std::vector<std::string>& foreign_key_proxy_keys) const;

    void update_proxy_dicts(
        Dictionary& foreign_key_dict,
        const VecKeyValuePair& fields,
        const std::vector<std::string>& string_get_keys,
        const std::vector<std::string>& foreign_key_proxy_keys) const;

    std::string get_item_key(const std::string& id) const;

    std::string get_field_key(
        const std::string& field, const std::string& value) const;

    void get_item_keys(
        const std::vector<std::string>& ids,
        std::vector<std::string>& keys) const;

    std::string get_set_key() const;

    std::string get_lock_key(
        const std::string& id, CrudLockType lock_type) const;

    std::string get_prefix() const;

    void error_check(
        const std::string& identifier, const BaseResponse* response) const;

    KeyValueStoreClient* m_client{nullptr};
};
}  // namespace hestia