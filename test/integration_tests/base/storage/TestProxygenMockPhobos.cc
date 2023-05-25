#include <string>

#ifdef HAVE_PROXYGEN

#include <catch2/catch_all.hpp>

#include "ProxygenServer.h"
#include "S3WebApp.h"
#include "TestContext.h"

#include "Logger.h"

TEST_CASE("Test Proxygen Mock Phobos", "[integration]")
{
    return;

    auto plugin =
        TestContext::get().get_object_store_plugin("mock_phobos_plugin");
    auto object_store = plugin->get_client();

    hestia::S3ServiceConfig service_config;
    service_config.m_object_store_client = object_store;

    auto s3_service = hestia::S3Service::create(service_config);

    hestia::S3WebAppConfig app_config;
    hestia::S3WebApp web_app(object_store, app_config, std::move(s3_service));

    hestia::Server::Config test_config;
    hestia::ProxygenServer server(test_config, &web_app);

    server.initialize();
    // server.start();
}
#endif