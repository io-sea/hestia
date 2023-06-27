#include <string>

#ifdef HAVE_PROXYGEN
#ifdef __HESTIA_DUMMY__

#include <catch2/catch_all.hpp>

#include "InMemoryKeyValueStoreClient.h"
#include "ProxygenServer.h"
#include "S3Service.h"
#include "TestContext.h"
#include "UserService.h"

#include "Logger.h"

TEST_CASE("Test Proxygen Mock Phobos", "[integration]")
{
    return;

    auto plugin =
        TestContext::get().get_object_store_plugin("mock_phobos_plugin");
    auto object_store = plugin->get_client();
    hestia::InMemoryKeyValueStoreClient m_kv_store_client;

    hestia::S3ServiceConfig service_config;
    service_config.m_object_store_client = object_store;

    auto s3_service   = hestia::S3Service::create(service_config);
    auto user_service = hestia::UserService::create({}, &m_kv_store_client);

    hestia::S3WebAppConfig app_config;
    hestia::S3WebApp web_app(
        object_store, user_service.get(), app_config, std::move(s3_service));

    hestia::Server::Config test_config;
    hestia::ProxygenServer server(test_config, &web_app);

    server.initialize();
    // server.start();
}
#endif
#endif