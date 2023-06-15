#include <catch2/catch_all.hpp>

#include "BasicHttpServer.h"
#include "CurlClient.h"
#include "MockWebView.h"
#include "UrlRouter.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "FileHsmObjectStoreClient.h"
#include "FileKeyValueStoreClient.h"

#include "DistributedHsmService.h"
#include "HestiaWebApp.h"
#include "HsmService.h"

#include "HsmObjectAdapter.h"
#include "StorageTierAdapter.h"

#include "TestUtils.h"

#include <filesystem>
#include <iostream>

TEST_CASE("Test Hestia Web App", "[hestia_web_app]")
{
    auto store_path =
        TestUtils::get_test_output_dir(__FILE__) / "TestHestiaWebWpp";
    std::filesystem::remove_all(store_path);
    hestia::Metadata store_config;
    store_config.set_item("root", store_path);

    hestia::FileKeyValueStoreClient kv_store_client;
    kv_store_client.initialize(store_config);

    hestia::FileHsmObjectStoreClient obj_store_client;
    obj_store_client.initialize(store_config);

    auto hsm_service =
        hestia::HsmService::create(&kv_store_client, &obj_store_client);

    hestia::HsmObjectStoreClientBackend object_store_backend(
        hestia::HsmObjectStoreClientBackend::Type::HSM,
        hestia::HsmObjectStoreClientBackend::Source::BUILT_IN,
        "hestia::FileHsmObjectStoreClient");

    hestia::DistributedHsmServiceConfig dist_hsm_config;
    dist_hsm_config.m_self.m_backends = {object_store_backend};
    auto dist_hsm_service             = hestia::DistributedHsmService::create(
        dist_hsm_config, std::move(hsm_service), &kv_store_client);

    hestia::HestiaWebApp web_app(dist_hsm_service.get());

    hestia::Server::Config server_config;
    hestia::BasicHttpServer server(server_config, &web_app);

    server.initialize();
    server.start();
    server.wait_until_bound();

    hestia::CurlClientConfig config;
    hestia::CurlClient http_client(config);

    const std::string base_url = "127.0.0.1:8000/api/v1/hsm/";

    const auto url = base_url + "objects";
    hestia::HttpRequest get_request(url, hestia::HttpRequest::Method::GET);

    auto get_response = http_client.make_request(get_request);
    REQUIRE(get_response->body() == "[]");

    hestia::HsmObject obj(1234);
    std::string obj_json;
    hestia::HsmObjectJsonAdapter obj_adapter{"hestia"};
    obj_adapter.to_string(obj, obj_json);

    hestia::HttpRequest put_request(url, hestia::HttpRequest::Method::PUT);
    put_request.body() = obj_json;
    auto put_response  = http_client.make_request(put_request);
    REQUIRE(!put_response->error());

    auto get_response1 = http_client.make_request(get_request);

    std::vector<hestia::HsmObject> objects;
    obj_adapter.from_string(get_response1->body(), objects);

    REQUIRE(objects.size() == 1);
    REQUIRE(objects[0].id() == hestia::Uuid(1234).to_string());

    const auto tiers_url = base_url + "tiers";
    hestia::HttpRequest tier_get_request(
        tiers_url, hestia::HttpRequest::Method::GET);

    auto tier_get_response = http_client.make_request(tier_get_request);
    REQUIRE(tier_get_response->body() == "[]");
}