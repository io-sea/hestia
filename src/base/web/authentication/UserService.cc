#include "UserService.h"

#include "HttpClient.h"
#include "HttpCrudClient.h"
#include "KeyValueCrudClient.h"
#include "KeyValueStoreClient.h"

#include "HashUtils.h"
#include "UserAdapter.h"

namespace hestia {

UserService::UserService(
    const UserServiceConfig& config, std::unique_ptr<CrudClient<User>> client) :
    CrudService<User>(std::move(client)), m_config(config)
{
}

UserService::~UserService() {}

UserService::Ptr UserService::create(
    const UserServiceConfig& config, KeyValueStoreClient* client)
{
    auto adapter = std::make_unique<UserJsonAdapter>(true);

    KeyValueCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client = std::make_unique<KeyValueCrudClient<User>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<UserService>(config, std::move(crud_client));
}

UserService::Ptr UserService::create(
    const UserServiceConfig& config, HttpClient* client)
{
    auto adapter = std::make_unique<UserJsonAdapter>();

    HttpCrudClientConfig crud_client_config;
    crud_client_config.m_item_prefix = config.m_item_prefix;
    crud_client_config.m_prefix      = config.m_global_prefix;
    auto crud_client                 = std::make_unique<HttpCrudClient<User>>(
        crud_client_config, std::move(adapter), client);

    return std::make_unique<UserService>(config, std::move(crud_client));
}

std::unique_ptr<UserServiceResponse> UserService::authenticate_user(
    const std::string& username, const std::string& password)
{
    User user(username);
    CrudRequest<User> request(user, CrudMethod::GET);
    auto get_response = make_request(request);
    if (!get_response->ok()) {
        return get_response;
    }

    const auto hashed_password = get_hashed_password(username, password);
    if (hashed_password == get_response->item().m_password) {
        return get_response;
    }
    else {
        auto response = std::make_unique<UserServiceResponse>(request);
        response->on_error(
            {CrudErrorCode::NOT_AUTHENTICATED, "Failed to authenticate user"});
        return response;
    }
}

std::unique_ptr<UserServiceResponse> UserService::register_user(
    const std::string& username, const std::string& password)
{
    User user(username);

    CrudRequest<User> request(user, CrudMethod::EXISTS);
    auto exists_response = make_request(request);
    if (!exists_response->ok()) {
        return exists_response;
    }

    if (exists_response->found()) {
        auto response = std::make_unique<UserServiceResponse>(request);
        response->on_error(
            {CrudErrorCode::CANT_OVERRIDE_EXISTING, "User already exists"});
        return response;
    }

    user.m_password = get_hashed_password(username, password);
    user.m_api_token.m_value =
        HashUtils::base64_encode(HashUtils::do_rand_32());

    auto put_response = make_request({user, CrudMethod::PUT});
    return put_response;
}

std::string UserService::get_hashed_password(
    const std::string& username, const std::string& password) const
{
    std::string salt = "hestia";
    return HashUtils::base64_encode(
        HashUtils::do_sha256(username + salt + password));
}

}  // namespace hestia