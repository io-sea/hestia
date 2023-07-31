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
    void create(const CrudRequest& request, CrudResponse& response) override;

    void read(const CrudQuery& query, CrudResponse& response) const override;

    void update(
        const CrudRequest& request, CrudResponse& response) const override;

    void remove(const VecCrudIdentifier& ids) const override;

    void identify(
        const VecCrudIdentifier& ids, CrudResponse& response) const override;

    void lock(const CrudIdentifier& id, CrudLockType lock_type) const override;

    void unlock(
        const CrudIdentifier& id, CrudLockType lock_type) const override;

    bool is_locked(
        const CrudIdentifier& id, CrudLockType lock_type) const override;

    void process_items(
        const CrudRequest& request,
        std::vector<std::string>& ids,
        std::vector<VecKeyValuePair>& index_fields,
        std::vector<VecKeyValuePair>& foregin_key_fields,
        Dictionary& content) const;

    void process_ids(
        const CrudRequest& request,
        std::vector<std::string>& ids,
        std::vector<VecKeyValuePair>& index_fields,
        std::vector<VecKeyValuePair>& foregin_key_fields,
        const Dictionary& attributes,
        Dictionary& content) const;

    void process_empty(
        std::vector<std::string>& ids,
        std::vector<VecKeyValuePair>& index_fields,
        std::vector<VecKeyValuePair>& foregin_key_fields,
        const Dictionary& attributes,
        Dictionary& content) const;

    void prepare_create_keys(
        std::vector<KeyValuePair>& string_set_kv_pairs,
        std::vector<KeyValuePair>& set_add_kv_pairs,
        std::vector<std::string>& ids,
        const std::vector<VecKeyValuePair>& index_fields,
        const std::vector<VecKeyValuePair>& foregin_key_fields,
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
        const std::string& identifier, BaseResponse* response) const;

    KeyValueStoreClient* m_client{nullptr};
};
}  // namespace hestia