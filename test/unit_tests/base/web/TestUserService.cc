#include <catch2/catch_all.hpp>

#include "InMemoryKeyValueStoreClient.h"
#include "UserService.h"

class TestUserServiceFixture {
  public:
    TestUserServiceFixture()
    {
        m_user_service = hestia::UserService::create({}, &m_kv_store_client);
    }

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    std::unique_ptr<hestia::UserService> m_user_service;
};

TEST_CASE_METHOD(TestUserServiceFixture, "Test User Service", "[user-service]")
{
    std::string username = "my_user";
    std::string password = "my_password";

    auto register_response = m_user_service->register_user(username, password);
    REQUIRE(register_response->ok());

    auto id = register_response->item().id();

    auto auth_response = m_user_service->authenticate_user(username, password);
    REQUIRE(auth_response->ok());

    REQUIRE(auth_response->item().id() == id);

    REQUIRE_FALSE(auth_response->item().m_api_token.m_value.empty());

    auto token_response = m_user_service->authenticate_with_token(
        auth_response->item().m_api_token.m_value);
    REQUIRE(token_response->ok());
}