#pragma once

#include "CrudService.h"
#include "CrudServiceBackend.h"
#include "User.h"
#include "UserTokenGenerator.h"

#include <memory>

namespace hestia {

class KeyValueStoreClient;
class HttpClient;
class IdGenerator;
class TimeProvider;

class UserService : public CrudService {
  public:
    using Ptr = std::unique_ptr<UserService>;
    UserService(
        const ServiceConfig& config,
        std::unique_ptr<CrudClient> client,
        CrudService::Ptr token_service,
        std::unique_ptr<UserTokenGenerator> token_generator = nullptr);

    virtual ~UserService();

    static Ptr create(
        const ServiceConfig& config,
        CrudServiceBackend* backend,
        IdGenerator* id_generator                           = nullptr,
        TimeProvider* time_provider                         = nullptr,
        std::unique_ptr<UserTokenGenerator> token_generator = nullptr);

    CrudResponse::Ptr authenticate_user(
        const std::string& username, const std::string& password) const;

    CrudResponse::Ptr authenticate_with_token(const std::string& token) const;

    CrudResponse::Ptr register_user(
        const std::string& username, const std::string& password) const;

  private:
    std::string get_hashed_password(
        const std::string& username, const std::string& password) const;

    CrudService::Ptr m_token_service;
    std::unique_ptr<UserTokenGenerator> m_token_generator;
};
}  // namespace hestia