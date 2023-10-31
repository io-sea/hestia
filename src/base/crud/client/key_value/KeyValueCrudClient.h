#pragma once

#include "CrudClient.h"
#include "Response.h"

namespace hestia {

class KeyValueStoreClient;

class KeyValueCrudClient : public CrudClient {
  public:
    KeyValueCrudClient(
        const CrudClientConfig& config,
        ModelSerializer::Ptr serializer,
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

    void prepare_creation_overrides(
        const CrudUserContext& user_context,
        Dictionary& creation_overrides) const;

    void prepare_update_overrides(Dictionary& update_overrides) const;

    void assign_modified_attributes(
        const Dictionary& content, CrudResponse& response) const;

    std::vector<std::string> get_db_items(
        const std::vector<std::string>& keys) const;

    void get_db_items(
        const std::vector<std::string>& keys, VecModelPtr& items) const;

    bool get_db_items(
        const std::vector<std::string>& keys,
        Dictionary& db_content,
        bool expects_single = false) const;

    std::string get_db_item(const std::string& key) const;

    void get_db_sets(
        const std::vector<std::string>& keys,
        std::vector<std::vector<std::string>>& values) const;

    std::string get_lock_key(
        const std::string& id, CrudLockType lock_type) const;

    void error_check(
        const std::string& identifier, const BaseResponse* response) const;

    KeyValueStoreClient* m_client{nullptr};
};
}  // namespace hestia