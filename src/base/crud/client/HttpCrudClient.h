#pragma once

#include "CrudClient.h"

namespace hestia {

class HttpClient;

class HttpCrudClient : public CrudClient {
  public:
    HttpCrudClient(
        const CrudClientConfig& config,
        AdapterCollectionPtr adapters,
        HttpClient* client);

    virtual ~HttpCrudClient();

    void create(const CrudRequest& request, CrudResponse& response) override;

    void read(
        const CrudRequest& request, CrudResponse& response) const override;

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

  protected:
    std::string get_item_path(const std::string& id) const;

    void get_item_keys(
        const std::vector<std::string>& ids,
        std::vector<std::string>& keys) const;

    std::string get_set_key() const;

    HttpClient* m_client{nullptr};
};
}  // namespace hestia