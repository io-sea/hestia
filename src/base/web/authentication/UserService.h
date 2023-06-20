#pragma once

#include "CrudService.h"
#include "User.h"

#include <memory>

namespace hestia {

class KeyValueStoreClient;
class HttpClient;

struct UserServiceConfig {
    std::string m_endpoint;
    std::string m_global_prefix;
    std::string m_item_prefix{"user"};
};

using UserServiceResponse = CrudResponse<User, CrudErrorCode>;

class UserService : public CrudService<User> {
  public:
    using Ptr = std::unique_ptr<UserService>;
    UserService(
        const UserServiceConfig& config,
        std::unique_ptr<CrudClient<User>> client);

    virtual ~UserService();

    static Ptr create(
        const UserServiceConfig& config, KeyValueStoreClient* client);

    static Ptr create(const UserServiceConfig& config, HttpClient* client);

    std::unique_ptr<UserServiceResponse> authenticate_user(
        const std::string& username, const std::string& password);

    std::unique_ptr<UserServiceResponse> register_user(
        const std::string& username, const std::string& password);

  private:
    std::string get_hashed_password(
        const std::string& username, const std::string& password) const;

    UserServiceConfig m_config;
};
}  // namespace hestia