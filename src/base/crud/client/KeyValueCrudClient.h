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
        const CrudRequest& request, CrudResponse& response) const override;

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