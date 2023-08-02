#include <catch2/catch_all.hpp>

#include "InMemoryKeyValueStoreClient.h"
#include "UserService.h"

#include "MockIdGenerator.h"
#include "MockTimeProvider.h"

#include <iostream>

class TestUserServiceFixture {
  public:
    TestUserServiceFixture()
    {
        m_id_generator  = std::make_unique<hestia::mock::MockIdGenerator>();
        m_time_provider = std::make_unique<hestia::mock::MockTimeProvider>();

        hestia::KeyValueStoreCrudServiceBackend backend(&m_kv_store_client);
        m_user_service = hestia::UserService::create(
            {}, &backend, m_id_generator.get(), m_time_provider.get());
    }

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    std::unique_ptr<hestia::UserService> m_user_service;
    std::unique_ptr<hestia::mock::MockIdGenerator> m_id_generator;
    std::unique_ptr<hestia::mock::MockTimeProvider> m_time_provider;
};

TEST_CASE_METHOD(TestUserServiceFixture, "Test User Service", "[user-service]")
{
    const std::string username = "my_user";
    const std::string password = "my_password";

    const auto register_response =
        m_user_service->register_user(username, password);

    REQUIRE(register_response->ok());

    const auto id = register_response->get_item()->id();

    const auto auth_response =
        m_user_service->authenticate_user(username, password);
    REQUIRE(auth_response->ok());

    REQUIRE(auth_response->get_item()->id() == id);

    const auto token_response = m_user_service->authenticate_with_token(
        auth_response->get_item_as<hestia::User>()->tokens()[0].value());
    REQUIRE(token_response->ok());

    REQUIRE(token_response->get_item()->id() == id);
}