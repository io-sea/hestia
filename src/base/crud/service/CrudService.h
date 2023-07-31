#pragma once

#include "CrudResponse.h"
#include "Model.h"
#include "Service.h"

namespace hestia {

class CrudClient;
class IdGenerator;
class TimeProvider;

class CrudService : public Service<CrudRequest, CrudResponse, CrudErrorCode> {
  public:
    CrudService(
        const ServiceConfig& config,
        std::unique_ptr<CrudClient> client          = {},
        std::unique_ptr<IdGenerator> id_generator   = nullptr,
        std::unique_ptr<TimeProvider> time_provider = nullptr);

    using Ptr = std::unique_ptr<CrudService>;

    virtual ~CrudService();

    std::string get_type() const;

    const std::string& get_default_name() const;

    [[nodiscard]] CrudResponse::Ptr make_request(
        const CrudRequest& request,
        const std::string& type = {}) const noexcept override;

    void register_parent_service(const std::string& type, CrudService* service);

    void register_child_service(const std::string& type, CrudService* service);

    void set_default_name(const std::string& name);

  protected:
    virtual void create(
        const CrudRequest& request, CrudResponse& response) const;

    virtual void read(const CrudRequest& request, CrudResponse& response) const;

    virtual void update(
        const CrudRequest& request, CrudResponse& response) const;

    virtual void remove(const VecCrudIdentifier& id) const;

    virtual void identify(
        const VecCrudIdentifier& id, CrudResponse& response) const;

    virtual void lock(const CrudIdentifier& id, CrudLockType lock_type) const;

    virtual void unlock(const CrudIdentifier& id, CrudLockType lock_type) const;

    virtual bool is_locked(
        const CrudIdentifier& id, CrudLockType lock_type) const;

  private:
    void on_exception(
        const CrudRequest& request,
        CrudResponse* response,
        const std::string& message = {}) const override;

    void on_exception(
        const CrudRequest& request,
        CrudResponse* response,
        const RequestError<CrudErrorCode>& error) const override;

    std::unique_ptr<CrudClient> m_client;
    std::unique_ptr<IdGenerator> m_id_generator;
    std::unique_ptr<TimeProvider> m_time_provider;
};

}  // namespace hestia
