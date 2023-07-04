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
#include "UserService.h"
#include "UuidUtils.h"

#include "TestUtils.h"

#include <filesystem>
#include <iostream>

class TestHestiaWebAppFixture {
  public:
    TestHestiaWebAppFixture()
    {
        m_store_path = TestUtils::get_test_output_dir(__FILE__);
        std::filesystem::remove_all(m_store_path);
        hestia::Metadata store_config;
        store_config.set_item("root", m_store_path);

        m_kv_store_client.initialize(store_config);
        m_obj_store_client.initialize(store_config);

        m_user_service = hestia::UserService::create({}, &m_kv_store_client);

        auto hsm_service =
            hestia::HsmService::create(&m_kv_store_client, &m_obj_store_client);

        hestia::HsmObjectStoreClientBackend object_store_backend(
            hestia::HsmObjectStoreClientBackend::Type::HSM,
            hestia::HsmObjectStoreClientBackend::Source::BUILT_IN,
            "hestia::FileHsmObjectStoreClient");

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.m_backends = {object_store_backend};
        m_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), &m_kv_store_client);

        m_object_adapter =
            std::make_unique<hestia::JsonAdapter<hestia::HsmObject>>();
        m_tier_adapter =
            std::make_unique<hestia::JsonAdapter<hestia::StorageTier>>();

        m_web_app = std::make_unique<hestia::HestiaWebApp>(
            m_user_service.get(), m_dist_hsm_service.get());

        hestia::Server::Config server_config;
        m_server = std::make_unique<hestia::BasicHttpServer>(
            server_config, m_web_app.get());

        m_server->initialize();
        m_server->start();
        m_server->wait_until_bound();

        hestia::CurlClientConfig http_config;
        m_http_client = std::make_unique<hestia::CurlClient>(http_config);
    }

    ~TestHestiaWebAppFixture() {}

    void get_objects(std::vector<hestia::HsmObject>& objects)
    {
        hestia::HttpRequest req(
            m_base_url + "objects", hestia::HttpRequest::Method::GET);
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        m_object_adapter->from_string(response->body(), objects);
    }

    void get_object(const hestia::Uuid& id, hestia::HsmObject& object)
    {
        const auto path =
            m_base_url + "objects/" + hestia::UuidUtils::to_string(id);
        hestia::HttpRequest req(path, hestia::HttpRequest::Method::GET);
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        m_object_adapter->from_string(response->body(), object);
    }

    void put_object(hestia::HsmObject& object)
    {
        hestia::HttpRequest req(
            m_base_url + "objects", hestia::HttpRequest::Method::PUT);
        m_object_adapter->to_string(object, req.body());
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        m_object_adapter->from_string(response->body(), object);
    }

    void put_data(
        const hestia::HsmObject& object,
        const std::string& content,
        uint8_t tier)
    {
        const auto path = m_base_url + "objects/"
                          + hestia::UuidUtils::to_string(object.id()) + "/"
                          + std::to_string(tier);

        hestia::HttpRequest req(path, hestia::HttpRequest::Method::PUT);
        req.body()    = content;
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
    }

    void get_data(
        const hestia::HsmObject& object, std::string& content, uint8_t tier)
    {
        const auto path = m_base_url + "objects/"
                          + hestia::UuidUtils::to_string(object.id()) + "/"
                          + std::to_string(tier);

        hestia::HttpRequest req(path, hestia::HttpRequest::Method::GET);
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());

        content = response->body();
    }

    void get_tiers(std::vector<hestia::StorageTier>& tiers)
    {
        hestia::HttpRequest req(
            m_base_url + "tiers", hestia::HttpRequest::Method::GET);
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        m_tier_adapter->from_string(response->body(), tiers);
    }

    void put_tier(const hestia::StorageTier& tier)
    {
        hestia::HttpRequest req(
            m_base_url + "tiers", hestia::HttpRequest::Method::PUT);
        m_tier_adapter->to_string(tier, req.body());
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
    }

    hestia::FileKeyValueStoreClient m_kv_store_client;
    hestia::FileHsmObjectStoreClient m_obj_store_client;
    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;
    std::unique_ptr<hestia::UserService> m_user_service;
    std::unique_ptr<hestia::JsonAdapter<hestia::HsmObject>> m_object_adapter;
    std::unique_ptr<hestia::JsonAdapter<hestia::StorageTier>> m_tier_adapter;

    std::unique_ptr<hestia::HestiaWebApp> m_web_app;
    std::unique_ptr<hestia::BasicHttpServer> m_server;
    std::unique_ptr<hestia::CurlClient> m_http_client;

    std::string m_base_url   = "127.0.0.1:8000/api/v1/hsm/";
    std::string m_store_path = {};
};

TEST_CASE_METHOD(
    TestHestiaWebAppFixture, "Test Hestia Web App", "[hestia_web_app]")
{
    std::vector<hestia::HsmObject> objects;
    get_objects(objects);
    REQUIRE(objects.empty());

    hestia::HsmObject obj;
    put_object(obj);

    hestia::HsmObject returned_object;
    get_object(obj.id(), returned_object);
    REQUIRE(returned_object.id() == obj.id());

    get_objects(objects);
    REQUIRE(objects.size() == 1);
    REQUIRE(objects[0].id() == obj.id());

    std::vector<hestia::StorageTier> tiers;
    get_tiers(tiers);
    REQUIRE(tiers.empty());

    hestia::StorageTier tier0(0);
    tier0.m_backend = "hestia::FileHsmObjectStoreClient";

    hestia::StorageTier tier1(1);
    tier1.m_backend = "hestia::FileHsmObjectStoreClient";
    put_tier(tier0);
    put_tier(tier1);

    get_tiers(tiers);
    REQUIRE(tiers.size() == 2);
    REQUIRE(tiers[0].m_backend == tier0.m_backend);

    std::string content{"The quick brown fox jumps over the lazy dog."};
    put_data(obj, content, 0);

    std::string returned_content;
    get_data(obj, returned_content, 0);
}