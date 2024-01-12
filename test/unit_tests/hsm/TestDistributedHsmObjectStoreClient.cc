#include <catch2/catch_all.hpp>

#include "MockHsmHttpClient.h"

#include "DistributedHsmObjectStoreClient.h"
#include "HsmService.h"
#include "HttpClient.h"
#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "ObjectStoreClientManager.h"
#include "TypedCrudRequest.h"
#include "UserService.h"

#include "TestUtils.h"

#include <future>
#include <iostream>

namespace hestia::mock {
class ObjectStoreClientWithRedirect : public InMemoryHsmObjectStoreClient {
  public:
    void get(HsmObjectStoreContext& ctx) const override
    {
        if (m_redirect.empty()) {
            InMemoryHsmObjectStoreClient::get(ctx);
        }
        else {
            LOG_INFO("Doing redirect to: " +  m_redirect);
            auto response = HsmObjectStoreResponse::create(ctx.m_request, "");
            response->object().set_location(m_redirect);
            ctx.finish(std::move(response));
        }
    }
    std::string m_redirect;
};

class TestObjectStoreClientManager : public ObjectStoreClientManager {
  public:
    TestObjectStoreClientManager(ObjectStoreClientFactory::Ptr client_factory) :
        ObjectStoreClientManager(std::move(client_factory))
    {
    }

    void replace_first_client(HsmObjectStoreClient::Ptr client)
    {
        m_backends[0]->m_hsm_object_store_client = std::move(client);
    }
};
}  // namespace hestia::mock

class DistributedHsmObjectStoreClientTestFixture {
  public:
    DistributedHsmObjectStoreClientTestFixture()
    {
        m_kv_store_client =
            std::make_unique<hestia::InMemoryKeyValueStoreClient>();

        setup_controller();
    }

    void setup_local_client()
    {
        LOG_INFO("Setting up client");
        auto client_manager =
            std::make_unique<hestia::ObjectStoreClientManager>(nullptr);
        m_local_object_store_client =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_http_client.get());

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());

        auto hsm_child_services =
            std::make_unique<hestia::HsmServiceCollection>();
        hsm_child_services->create_default_services(
            {}, &crud_backend, m_user_service.get());

        hsm_child_services->get_service(hestia::HsmItem::Type::DATASET)
            ->set_default_name("test_default_dataset");

        auto hsm_service = std::make_unique<hestia::HsmService>(
            hestia::ServiceConfig{}, std::move(hsm_child_services),
            m_local_object_store_client.get());
        hsm_service->update_tiers(m_test_user.get_primary_key());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_controller_address = "45678";
        m_local_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), m_user_service.get());

        m_local_object_store_client->do_initialize(
            {}, m_local_dist_hsm_service.get());
    }

    void setup_controller()
    {
        LOG_INFO("Setting up controller");
        m_http_client = std::make_unique<MockHsmHttpClient>();

        auto client_manager =
            std::make_unique<hestia::ObjectStoreClientManager>(nullptr);

        m_controller_object_store_client =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_http_client.get());

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());

        m_user_service = hestia::UserService::create({}, &crud_backend);
        auto register_response =
            m_user_service->register_user("my_admin", "my_admin_password");
        REQUIRE(register_response->ok());

        auto auth_response =
            m_user_service->authenticate_user("my_admin", "my_admin_password");
        REQUIRE(auth_response->ok());
        m_test_user = *auth_response->get_item_as<hestia::User>();

        auto hsm_child_services =
            std::make_unique<hestia::HsmServiceCollection>();
        hsm_child_services->create_default_services(
            {}, &crud_backend, m_user_service.get());

        hsm_child_services->get_service(hestia::HsmItem::Type::DATASET)
            ->set_default_name("test_default_dataset");

        auto tier_service =
            hsm_child_services->get_service(hestia::HsmItem::Type::TIER);

        for (std::size_t idx = 0; idx < 5; idx++) {
            hestia::StorageTier tier(std::to_string(idx));
            tier.set_priority(idx);
            auto response = tier_service->make_request(hestia::TypedCrudRequest{
                hestia::CrudMethod::CREATE,
                tier,
                {},
                {m_test_user.get_primary_key()}});
            REQUIRE(response->ok());
        }

        auto hsm_service = std::make_unique<hestia::HsmService>(
            hestia::ServiceConfig{}, std::move(hsm_child_services),
            m_controller_object_store_client.get());
        hsm_service->update_tiers(m_test_user.get_primary_key());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.set_is_controller(true);
        dist_hsm_config.m_self.set_host_address("45678");
        dist_hsm_config.m_self.set_name("my_controller");
        dist_hsm_config.m_is_server = true;

        m_controller_dist_hsm_service = hestia::DistributedHsmService::create(
            dist_hsm_config, std::move(hsm_service), m_user_service.get());

        m_controller_dist_hsm_service->register_self();

        m_http_client->add_app(m_controller_dist_hsm_service.get(), "45678");

        m_controller_object_store_client->do_initialize(
            {}, m_controller_dist_hsm_service.get());
    }

    void setup_worker(
        const std::string& address      = "12345",
        const std::string& redirect_loc = {})
    {
        LOG_INFO("Setting up worker");
        auto client_factory =
            std::make_unique<hestia::ObjectStoreClientFactory>(nullptr);
        auto client_manager =
            std::make_unique<hestia::mock::TestObjectStoreClientManager>(
                std::move(client_factory));

        auto raw_client_manager = client_manager.get();

        m_worker_object_store_clients[address] =
            std::make_unique<hestia::DistributedHsmObjectStoreClient>(
                std::move(client_manager), m_http_client.get());

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());

        auto hsm_child_services =
            std::make_unique<hestia::HsmServiceCollection>();
        hsm_child_services->create_default_services(
            {}, &crud_backend, m_user_service.get());

        hsm_child_services->get_service(hestia::HsmItem::Type::DATASET)
            ->set_default_name("test_default_dataset");

        auto hsm_service = std::make_unique<hestia::HsmService>(
            hestia::ServiceConfig{}, std::move(hsm_child_services),
            m_worker_object_store_clients[address].get());
        hsm_service->update_tiers(m_test_user.get_primary_key());

        hestia::DistributedHsmServiceConfig dist_hsm_config;
        dist_hsm_config.m_self.set_is_controller(false);
        dist_hsm_config.m_controller_address = "45678";
        dist_hsm_config.m_self.set_host_address(address);
        dist_hsm_config.m_self.set_name("my_worker" + address);
        dist_hsm_config.m_is_server = true;

        hestia::InMemoryObjectStoreClientConfig hsm_memory_client_config;
        hestia::Dictionary serialized_config;
        hsm_memory_client_config.serialize(serialized_config);

        hestia::ObjectStoreBackend object_store_backend(
            hestia::ObjectStoreBackend::Type::MEMORY_HSM);
        object_store_backend.set_tier_ids({"0", "1", "2", "3", "4"});
        object_store_backend.set_config(serialized_config);

        dist_hsm_config.m_backends = {object_store_backend};

        m_worker_dist_hsm_services[address] =
            hestia::DistributedHsmService::create(
                dist_hsm_config, std::move(hsm_service), m_user_service.get());

        m_worker_dist_hsm_services[address]->register_self();

        m_http_client->add_app(
            m_worker_dist_hsm_services[address].get(), address);

        m_worker_object_store_clients[address]->do_initialize(
            {}, m_worker_dist_hsm_services[address].get());

        auto override_client =
            std::make_unique<hestia::mock::ObjectStoreClientWithRedirect>();
        override_client->m_redirect = redirect_loc;
        override_client->set_tier_ids({"0", "1", "2", "3", "4"});
        override_client->initialize("override_client", {}, {});
        raw_client_manager->replace_first_client(std::move(override_client));
    }

    void create_object(const std::string& id)
    {
        auto create_response =
            m_local_dist_hsm_service->get_hsm_service()->make_request(
                hestia::CrudRequest{
                    hestia::CrudMethod::CREATE,
                    {hestia::CrudIdentifier(id)},
                    {m_test_user.get_primary_key()}},
                hestia::HsmItem::hsm_object_name);
    }

    void put_data(
        const hestia::StorageObject& obj,
        const std::string& content,
        uint8_t tier,
        const std::string& preferred_address = {})
    {
        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::PUT_DATA);
        action.set_subject_key(obj.get_primary_key());
        action.set_target_tier(tier);
        action.set_preferred_node_address(preferred_address);

        std::promise<hestia::HsmActionResponse::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::HsmActionResponse::Ptr response) {
                response_promise.set_value(std::move(response));
            };

        hestia::Stream stream;
        stream.set_source(hestia::InMemoryStreamSource::create(
            hestia::ReadableBufferView{content}));
        m_local_dist_hsm_service->do_hsm_action(
            hestia::HsmActionRequest(action, {m_test_user.get_primary_key()}),
            &stream, completion_cb);

        if (stream.waiting_for_content()) {
            LOG_INFO("Going to flush stream");
            REQUIRE(stream.flush().ok());
        }

        const auto response = response_future.get();
        REQUIRE(response->ok());
    }

    void get_data(
        const hestia::StorageObject& obj,
        std::string& content,
        std::size_t content_length,
        uint8_t tier,
        const std::string& preferred_address = {})
    {
        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::GET_DATA);
        action.set_subject_key(obj.get_primary_key());
        action.set_source_tier(tier);
        action.set_preferred_node_address(preferred_address);

        std::promise<hestia::HsmActionResponse::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::HsmActionResponse::Ptr response) {
                response_promise.set_value(std::move(response));
            };

        hestia::Stream stream;
        std::vector<char> return_buffer(content_length);
        hestia::WriteableBufferView writeable_buffer(return_buffer);
        stream.set_sink(hestia::InMemoryStreamSink::create(writeable_buffer));

        m_local_dist_hsm_service->do_hsm_action(
            hestia::HsmActionRequest(action, {m_test_user.get_primary_key()}),
            &stream, completion_cb);

        if (stream.has_content()) {
            LOG_INFO("Going to flush stream");
            REQUIRE(stream.flush().ok());
        }

        const auto response = response_future.get();
        REQUIRE(response->ok());

        content = std::string(return_buffer.begin(), return_buffer.end());
    }

    std::unique_ptr<hestia::InMemoryKeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<MockHsmHttpClient> m_http_client;

    hestia::DistributedHsmObjectStoreClient::Ptr m_local_object_store_client;

    std::
        unordered_map<std::string, hestia::DistributedHsmObjectStoreClient::Ptr>
            m_worker_object_store_clients;
    hestia::DistributedHsmObjectStoreClient::Ptr
        m_controller_object_store_client;

    hestia::DistributedHsmService::Ptr m_local_dist_hsm_service;

    std::unordered_map<std::string, hestia::DistributedHsmService::Ptr>
        m_worker_dist_hsm_services;
    hestia::DistributedHsmService::Ptr m_controller_dist_hsm_service;

    std::unique_ptr<hestia::UserService> m_user_service;
    hestia::User m_test_user;
};

TEST_CASE_METHOD(
    DistributedHsmObjectStoreClientTestFixture,
    "Test Distributed Hsm Object Store Client",
    "[hsm]")
{
    setup_worker();
    setup_local_client();

    std::string id{"0000"};
    create_object(id);

    hestia::StorageObject obj(id);
    obj.set_metadata("mykey", "myval");

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.size());

    put_data(obj, content, 0);

    std::string tier0_content;
    hestia::StorageObject obj0(id);
    get_data(obj0, tier0_content, content.length(), 0);
    REQUIRE(tier0_content == content);
}

TEST_CASE_METHOD(
    DistributedHsmObjectStoreClientTestFixture,
    "Test Distributed Hsm Object Store Client - Object Store redirect",
    "[hsm]")
{
    setup_worker("1234");

    // Here we set up a 'stub' worker - whose only purpose is to
    // redirect a request to the real worker via its object store.
    setup_worker("891011", "1234");
    setup_local_client();

    std::string id{"0000"};
    create_object(id);

    hestia::StorageObject obj(id);
    obj.set_metadata("mykey", "myval");

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.set_size(content.size());

    put_data(obj, content, 0, "1234");

    std::string tier0_content;
    hestia::StorageObject obj0(id);
    get_data(obj0, tier0_content, content.length(), 0, "891011");
    REQUIRE(tier0_content == content);
}