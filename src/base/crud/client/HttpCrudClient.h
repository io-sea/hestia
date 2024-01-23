#pragma once

#include "CrudClient.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

namespace hestia {

class HttpClient;

class HttpCrudClient : public CrudClient {
  public:
    HttpCrudClient(
        const CrudClientConfig& config,
        ModelSerializer::Ptr serializer,
        HttpClient* client);

    virtual ~HttpCrudClient();

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

  protected:
    std::string get_item_path(const std::string& id = {}) const;

    std::string get_item_path(const CrudIdentifier& id, Map& queries) const;

    void get_item_keys(
        const std::vector<std::string>& ids,
        std::vector<std::string>& keys) const;

    std::string get_set_key() const;

    HttpClient* m_client{nullptr};

  private:
    void make_request(
        const CrudRequest& req,
        HttpRequest::Method method,
        CrudResponse& crud_response,
        bool except_on_error = true) const;

    HttpResponse::Ptr make_request(
        const HttpRequest& req, bool except_on_error = true) const;

    std::string get_query_child_format(CrudQuery::ChildFormat format) const;
};
}  // namespace hestia