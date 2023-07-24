#include <catch2/catch_all.hpp>

#include "CurlClient.h"
#include "MockWebView.h"
#include "UrlRouter.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"

#include "DistributedHsmService.h"
#include "HestiaWebApp.h"
#include "HsmService.h"
#include "HsmServicesFactory.h"
#include "StorageTier.h"
#include "StringAdapter.h"
#include "UserService.h"
#include "UuidUtils.h"

#include "TestUtils.h"

#include <filesystem>
#include <iostream>

#ifdef HAVE_PROXYGEN
#include "ProxygenServer.h"
using TestServer = hestia::ProxygenServer;
#else
#include "BasicHttpServer.h"
using TestServer = hestia::BasicHttpServer;
#endif

class TestHestiaWebAppFixture {
  public:
    TestHestiaWebAppFixture()
    {
        hestia::KeyValueStoreCrudServiceBackend backend(&m_kv_store_client);
        m_user_service = hestia::UserService::create({}, &backend);

        auto hsm_service = hestia::HsmService::create(
            hestia::ServiceConfig{}, &m_kv_store_client, &m_obj_store_client,
            m_user_service.get());

        hestia::HsmObjectStoreClientBackend object_store_backend(
            hestia::HsmObjectStoreClientBackend::Type::MEMORY,
            hestia::InMemoryHsmObjectStoreClient::get_registry_identifier());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.add_backend(object_store_backend);
        m_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), &backend,
            m_user_service.get());

        m_object_factory =
            std::make_unique<hestia::TypedModelFactory<hestia::HsmObject>>();
        m_tier_factory =
            std::make_unique<hestia::TypedModelFactory<hestia::StorageTier>>();
        m_object_adapter =
            std::make_unique<hestia::JsonAdapter>(m_object_factory.get());
        m_tier_adapter =
            std::make_unique<hestia::JsonAdapter>(m_tier_factory.get());

        m_web_app = std::make_unique<hestia::HestiaWebApp>(
            m_user_service.get(), m_dist_hsm_service.get());

        hestia::Server::Config server_config;
        m_server = std::make_unique<TestServer>(server_config, m_web_app.get());

        m_server->initialize();
        m_server->start();
        m_server->wait_until_bound();

        hestia::CurlClientConfig http_config;
        m_http_client = std::make_unique<hestia::CurlClient>(http_config);
    }

    ~TestHestiaWebAppFixture() { m_server->stop(); }

    void get_objects(std::vector<hestia::Model::Ptr>& objects)
    {
        hestia::HttpRequest req(
            m_base_url + "objects", hestia::HttpRequest::Method::GET);
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        m_object_adapter->from_string({response->body()}, objects);
    }

    void get_object(const std::string& id, hestia::HsmObject& object)
    {
        const auto path = m_base_url + "objects/" + id;
        hestia::HttpRequest req(path, hestia::HttpRequest::Method::GET);
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        std::vector<hestia::HsmObject::Ptr> objects;
        m_object_adapter->from_string({response->body()}, objects);
        object = *(dynamic_cast<hestia::HsmObject*>(objects[0].get()));
    }

    void put_object(hestia::HsmObject& object)
    {
        hestia::HttpRequest req(
            m_base_url + "objects", hestia::HttpRequest::Method::PUT);
        std::vector<hestia::HsmObject::Ptr> put_objects;
        put_objects.push_back(std::make_unique<hestia::HsmObject>(object));

        m_object_adapter->to_string(put_objects, req.body());
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        std::vector<hestia::HsmObject::Ptr> objects;
        m_object_adapter->from_string({response->body()}, objects);
        object = *(dynamic_cast<hestia::HsmObject*>(objects[0].get()));
    }

    void put_data(
        const hestia::HsmObject& object,
        const std::string& content,
        uint8_t tier)
    {
        const auto path = m_base_url + hestia::HsmItem::hsm_action_name + "s";
        hestia::HttpRequest req(path, hestia::HttpRequest::Method::PUT);

        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::PUT_DATA);
        action.set_target_tier(tier);
        action.set_subject_key(object.get_primary_key());
        action.set_size(content.size());

        hestia::Dictionary action_dict;
        action.serialize(action_dict);

        hestia::Map action_map;
        action_dict.flatten(action_map);
        action_map.add_key_prefix("hestia::hsm_action::");

        req.get_header().set_items(action_map);
        req.body()    = content;
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
    }

    void get_data(
        const hestia::HsmObject& object, std::string& content, uint8_t tier)
    {
        const auto path =
            m_base_url + "objects/" + object.id() + "/" + std::to_string(tier);

        hestia::HttpRequest req(path, hestia::HttpRequest::Method::GET);
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());

        content = response->body();
    }

    void get_tiers(std::vector<hestia::Model::Ptr>& tiers)
    {
        hestia::HttpRequest req(
            m_base_url + "tiers", hestia::HttpRequest::Method::GET);
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        m_tier_adapter->from_string({response->body()}, tiers);
    }

    void put_tier(const hestia::StorageTier& tier)
    {
        hestia::HttpRequest req(
            m_base_url + "tiers", hestia::HttpRequest::Method::PUT);

        std::vector<hestia::StorageTier::Ptr> put_tiers;
        put_tiers.push_back(std::make_unique<hestia::StorageTier>(tier));

        m_tier_adapter->to_string(put_tiers, req.body());
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
    }

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    hestia::InMemoryHsmObjectStoreClient m_obj_store_client;
    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;
    std::unique_ptr<hestia::UserService> m_user_service;

    std::unique_ptr<hestia::TypedModelFactory<hestia::HsmObject>>
        m_object_factory;
    std::unique_ptr<hestia::TypedModelFactory<hestia::StorageTier>>
        m_tier_factory;
    std::unique_ptr<hestia::JsonAdapter> m_object_adapter;
    std::unique_ptr<hestia::JsonAdapter> m_tier_adapter;

    std::unique_ptr<hestia::HestiaWebApp> m_web_app;
    std::unique_ptr<TestServer> m_server;
    std::unique_ptr<hestia::CurlClient> m_http_client;

    std::string m_base_url   = "127.0.0.1:8000/api/v1/";
    std::string m_store_path = {};
};

TEST_CASE_METHOD(
    TestHestiaWebAppFixture, "Test Hestia Web App", "[hestia_web_app]")
{
    std::vector<hestia::Model::Ptr> objects;
    get_objects(objects);
    REQUIRE(objects.empty());

    hestia::HsmObject obj;
    put_object(obj);

    hestia::HsmObject returned_object;
    get_object(obj.id(), returned_object);
    REQUIRE(returned_object.id() == obj.id());

    get_objects(objects);
    REQUIRE(objects.size() == 1);
    REQUIRE(objects[0]->id() == obj.id());

    std::vector<hestia::StorageTier::Ptr> tiers;
    get_tiers(tiers);
    REQUIRE(tiers.empty());

    hestia::StorageTier tier0(0);
    tier0.set_backend("hestia::FileHsmObjectStoreClient");

    hestia::StorageTier tier1(1);
    tier1.set_backend("hestia::FileHsmObjectStoreClient");
    put_tier(tier0);
    put_tier(tier1);

    get_tiers(tiers);
    REQUIRE(tiers.size() == 2);

    auto tiers_0 = dynamic_cast<hestia::StorageTier*>(tiers[0].get());

    REQUIRE(tiers_0->get_backend() == tier0.get_backend());

    /*
    // TODO Basic server doesn't handle large headers coming in chunks - need to
    fix that. std::string content{"The quick brown fox jumps over the lazy
    dog."}; put_data(obj, content, 0);

    std::string returned_content;
    get_data(obj, returned_content, 0);
    */
}