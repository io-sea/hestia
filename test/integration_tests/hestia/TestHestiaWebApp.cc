#include <catch2/catch_all.hpp>

#include "CurlClient.h"
#include "MockWebView.h"
#include "Stream.h"
#include "UrlRouter.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"

#include "DataPlacementEngine.h"
#include "DistributedHsmService.h"
#include "EventFeed.h"
#include "HestiaWebApp.h"
#include "HsmService.h"
#include "HsmServicesFactory.h"
#include "StorageTier.h"
#include "StringAdapter.h"
#include "TypedCrudRequest.h"
#include "UserService.h"
#include "UuidUtils.h"

#include "ProxygenTestUtils.h"
#include "TestUtils.h"

#include <filesystem>
#include <iostream>

class TestHestiaWebAppFixture {
  public:
    TestHestiaWebAppFixture()
    {
        m_kv_store_client =
            std::make_unique<hestia::InMemoryKeyValueStoreClient>();
        m_obj_store_client =
            std::make_unique<hestia::InMemoryHsmObjectStoreClient>();

        hestia::InMemoryObjectStoreClientConfig obj_store_config;
        m_obj_store_client->set_tier_names({"0", "1"});
        m_obj_store_client->do_initialize("0000", {}, obj_store_config);

        hestia::KeyValueStoreCrudServiceBackend backend(
            m_kv_store_client.get());
        m_user_service = hestia::UserService::create({}, &backend);

        auto register_response =
            m_user_service->register_user("my_admin", "my_admin_password");
        REQUIRE(register_response->ok());

        auto auth_response =
            m_user_service->authenticate_user("my_admin", "my_admin_password");
        REQUIRE(auth_response->ok());

        hestia::ObjectStoreBackend object_store_backend(
            hestia::ObjectStoreBackend::Type::MEMORY_HSM);

        auto hsm_child_services =
            std::make_unique<hestia::HsmServiceCollection>();
        hsm_child_services->create_default_services(
            {}, &backend, m_user_service.get(), nullptr);
        hsm_child_services->get_service(hestia::HsmItem::Type::DATASET)
            ->set_default_name("test_default_dataset");

        auto tier_service =
            hsm_child_services->get_service(hestia::HsmItem::Type::TIER);
        for (uint8_t idx = 0; idx < 2; idx++) {
            hestia::StorageTier tier(idx);
            auto create_response = tier_service->make_request(
                hestia::TypedCrudRequest<hestia::StorageTier>{
                    hestia::CrudMethod::CREATE, tier,
                    m_user_service->get_current_user().get_primary_key()});
            REQUIRE(create_response->ok());
        }

        hestia::ServiceConfig hsm_service_config;
        auto hsm_service = std::make_unique<hestia::HsmService>(
            hsm_service_config, std::move(hsm_child_services),
            m_obj_store_client.get());
        hsm_service->update_tiers(
            m_user_service->get_current_user().get_primary_key());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.set_name("my_node");
        dist_hsm_config.m_backends = {object_store_backend};

        m_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), m_user_service.get());

        m_dist_hsm_service->register_self();

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
        req.get_header().set_content_type("application/json");
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
        m_object_adapter->from_string({response->body()}, objects);
    }

    void get_object(const std::string& id, hestia::HsmObject& object)
    {
        const auto path = m_base_url + "objects/" + id;
        hestia::HttpRequest req(path, hestia::HttpRequest::Method::GET);
        req.get_header().set_content_type("application/json");

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
        req.get_header().set_auth_token(
            m_user_service->get_current_user().tokens()[0].value());
        req.get_header().set_content_type("application/json");

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
        action_map.add_key_prefix("hestia.hsm_action.");

        req.get_header().set_items(action_map);
        req.get_header().set_auth_token(
            m_user_service->get_current_user().tokens()[0].value());
        req.body()    = content;
        auto response = m_http_client->make_request(req);
        REQUIRE(!response->error());
    }

    void get_data(
        const hestia::HsmObject& object, std::string& content, uint8_t tier)
    {
        hestia::WriteableBufferView writeable_buffer(
            content.data(), content.size());
        hestia::Stream stream;
        auto sink =
            std::make_unique<hestia::InMemoryStreamSink>(writeable_buffer);
        stream.set_sink(std::move(sink));

        const auto path = m_base_url + hestia::HsmItem::hsm_action_name + "s";
        hestia::HttpRequest req(path, hestia::HttpRequest::Method::GET);

        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::GET_DATA);
        action.set_source_tier(tier);
        action.set_subject_key(object.get_primary_key());
        action.set_size(content.size());

        hestia::Dictionary action_dict;
        action.serialize(action_dict);

        hestia::Map action_map;
        action_dict.flatten(action_map);
        action_map.add_key_prefix("hestia.hsm_action.");

        req.get_header().set_items(action_map);
        req.get_header().set_auth_token(
            m_user_service->get_current_user().tokens()[0].value());

        auto response = m_http_client->make_request(req, &stream);
        REQUIRE(!response->error());
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

    std::unique_ptr<hestia::InMemoryKeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::InMemoryHsmObjectStoreClient> m_obj_store_client;
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
    REQUIRE(tiers.size() == 2);

    std::string content = "The quick brown fox jumps over the lazy dog.";
    put_data(obj, content, 0);

    std::string returned_content(content.size(), 0);
    get_data(obj, returned_content, 0);
    REQUIRE(returned_content == content);
}