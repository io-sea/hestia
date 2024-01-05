#include <catch2/catch_all.hpp>

#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "TypedCrudRequest.h"

#include "HsmService.h"
#include "HsmServicesFactory.h"
#include "StorageTier.h"
#include "UserService.h"

#include "TestUtils.h"

#include <future>
#include <iostream>

class HsmServiceTestFixture {
  public:
    HsmServiceTestFixture()
    {
        m_kv_store_client =
            std::make_unique<hestia::InMemoryKeyValueStoreClient>();

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            m_kv_store_client.get());
        m_user_service = hestia::UserService::create({}, &crud_backend);

        m_test_user.set_name("test_user");
        auto user_create_response =
            m_user_service->make_request(hestia::TypedCrudRequest<hestia::User>(
                hestia::CrudMethod::CREATE, m_test_user,
                {hestia::CrudQuery::BodyFormat::ITEM}, {}));
        REQUIRE(user_create_response->ok());
        m_test_user = *user_create_response->get_item_as<hestia::User>();

        auto hsm_child_services =
            std::make_unique<hestia::HsmServiceCollection>();
        hsm_child_services->create_default_services(
            {}, &crud_backend, m_user_service.get());

        hsm_child_services->get_service(hestia::HsmItem::Type::DATASET)
            ->set_default_name("test_default_dataset");

        auto tier_service =
            hsm_child_services->get_service(hestia::HsmItem::Type::TIER);

        hestia::InMemoryObjectStoreClientConfig object_store_config;

        std::vector<std::string> tier_ids;
        for (std::size_t idx = 0; idx < 5; idx++) {
            hestia::StorageTier tier(std::to_string(idx));
            tier.set_priority(idx);

            auto response = tier_service->make_request(
                hestia::TypedCrudRequest<hestia::StorageTier>{
                    hestia::CrudMethod::CREATE,
                    tier,
                    {},
                    m_test_user.get_primary_key()});
            REQUIRE(response->ok());
            tier_ids.push_back(std::to_string(idx));
        }

        m_object_store_client =
            std::make_unique<hestia::InMemoryHsmObjectStoreClient>();
        m_object_store_client->set_tier_ids(tier_ids);
        m_object_store_client->do_initialize("0000", {}, object_store_config);

        m_hsm_service = std::make_unique<hestia::HsmService>(
            hestia::ServiceConfig{}, std::move(hsm_child_services),
            m_object_store_client.get());
        m_hsm_service->update_tiers(m_test_user.get_primary_key());
    }

    void do_crud(const hestia::HsmObject& obj, hestia::CrudMethod method)
    {
        REQUIRE(
            m_hsm_service
                ->make_request(
                    hestia::TypedCrudRequest<hestia::HsmObject>{
                        method, obj,
                        hestia::CrudUserContext(m_test_user.get_primary_key())},
                    hestia::HsmItem::hsm_object_name)
                ->ok());
    }

    void create(const hestia::HsmObject& obj)
    {
        do_crud(obj, hestia::CrudMethod::CREATE);
    }

    void update(const hestia::HsmObject& obj)
    {
        do_crud(obj, hestia::CrudMethod::UPDATE);
    }

    bool exists(const hestia::HsmObject& obj)
    {
        hestia::CrudQuery query(
            obj.get_primary_key(), hestia::CrudQuery::BodyFormat::ITEM);
        auto exists = m_hsm_service->make_request(
            hestia::CrudRequest(hestia::CrudMethod::READ, query, {}),
            hestia::HsmItem::hsm_object_name);
        REQUIRE(exists->ok());
        return exists->found();
    }

    void put_data(
        const hestia::HsmObject& obj, const std::string& content, uint8_t tier)
    {
        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::PUT_DATA);
        action.set_subject_key(obj.get_primary_key());
        action.set_target_tier(tier);

        std::promise<hestia::HsmActionResponse::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::HsmActionResponse::Ptr response) {
                response_promise.set_value(std::move(response));
            };

        hestia::Stream stream;
        stream.set_source(hestia::InMemoryStreamSource::create(
            hestia::ReadableBufferView{content}));
        m_hsm_service->do_hsm_action(
            hestia::HsmActionRequest(action, {m_test_user.get_primary_key()}),
            &stream, completion_cb);

        LOG_INFO("Going to flush stream");
        REQUIRE(stream.flush().ok());

        const auto response = response_future.get();
        REQUIRE(response->ok());
    }

    void get_data(
        const hestia::HsmObject& obj,
        std::string& content,
        std::size_t content_length,
        uint8_t tier)
    {
        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT, hestia::HsmAction::Action::GET_DATA);
        action.set_subject_key(obj.get_primary_key());
        action.set_source_tier(tier);

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

        m_hsm_service->do_hsm_action(
            hestia::HsmActionRequest(action, {m_test_user.get_primary_key()}),
            &stream, completion_cb);

        LOG_INFO("Going to flush stream");
        REQUIRE(stream.flush().ok());

        const auto response = response_future.get();
        REQUIRE(response->ok());

        content = std::string(return_buffer.begin(), return_buffer.end());
    }

    void do_copy_or_move(
        const hestia::HsmObject& obj, int src_tier, int tgt_tier, bool is_copy)
    {
        hestia::HsmAction action(
            hestia::HsmItem::Type::OBJECT,
            is_copy ? hestia::HsmAction::Action::COPY_DATA :
                      hestia::HsmAction::Action::MOVE_DATA);
        action.set_source_tier(src_tier);
        action.set_target_tier(tgt_tier);
        action.set_subject_key(obj.get_primary_key());

        hestia::HsmActionResponse::Ptr response;
        auto completion_cb =
            [&response](hestia::HsmActionResponse::Ptr completion_response) {
                response = std::move(completion_response);
            };

        m_hsm_service->do_hsm_action(
            hestia::HsmActionRequest(action, {m_test_user.get_primary_key()}),
            nullptr, completion_cb);
        REQUIRE(response->ok());
    }


    void copy(const hestia::HsmObject& obj, int src_tier, int tgt_tier)
    {
        do_copy_or_move(obj, src_tier, tgt_tier, true);
    }

    void move(const hestia::HsmObject& obj, int src_tier, int tgt_tier)
    {
        do_copy_or_move(obj, src_tier, tgt_tier, false);
    }

    /*
    void remove(const hestia::HsmObject& obj, int tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::RELEASE_DATA);
        request.set_source_tier(tier);
        REQUIRE(m_hsm_service->make_request(request)->ok());
    }

    void remove_all(const hestia::HsmObject& obj)
    {
        REQUIRE(
            m_hsm_service
                ->make_request({obj, hestia::HsmServiceRequestMethod::RELEASE})
                ->ok());
    }

    void list_objects(std::vector<hestia::Uuid>& ids, uint8_t tier)
    {
        hestia::HsmServiceRequest request(
            tier, hestia::HsmServiceRequestSubject::OBJECT,
            hestia::HsmServiceRequestMethod::LIST);
        request.set_source_tier(tier);
        request.tier() = tier;
        auto response  = m_hsm_service->make_request(request);
        REQUIRE(response->ok());
        for (const auto& object_id : response->objects()) {
            ids.push_back(object_id.id());
        }
    }

    */

    bool is_object_on_tier(const hestia::HsmObject& obj, int tier_id)
    {
        hestia::CrudQuery query(
            hestia::CrudIdentifier(obj.get_primary_key()),
            hestia::CrudQuery::BodyFormat::ITEM);

        auto response = m_hsm_service->make_request(
            hestia::CrudRequest(hestia::CrudMethod::READ, query, {}),
            hestia::HsmItem::hsm_object_name);
        REQUIRE(response->ok());

        auto object = response->get_item_as<hestia::HsmObject>();
        for (const auto& extent : object->tiers()) {
            if (extent.get_tier_id() == std::to_string(tier_id)) {
                return true;
            }
        }
        return false;
    }

    std::unique_ptr<hestia::InMemoryKeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::InMemoryHsmObjectStoreClient> m_object_store_client;
    std::unique_ptr<hestia::UserService> m_user_service;
    std::unique_ptr<hestia::HsmService> m_hsm_service;
    hestia::User m_test_user;
};

TEST_CASE_METHOD(HsmServiceTestFixture, "Test HSM Service", "[hsm-service]")
{
    std::string id_0 = "0000";
    hestia::HsmObject obj0(id_0);

    create(obj0);

    uint8_t tier0_id          = 0;
    const std::string content = "The quick brown fox jumps over the lazy dog.";
    put_data(obj0, content, tier0_id);

    REQUIRE(exists(obj0));

    std::string id1{"0001"};
    std::string id2{"0002"};
    hestia::HsmObject obj1(id1);
    hestia::HsmObject obj2(id2);

    create(obj1);
    put_data(obj1, content, tier0_id);
    REQUIRE(exists(obj1));

    create(obj2);
    put_data(obj2, content, tier0_id);
    REQUIRE(exists(obj2));

    std::string tier0_content;
    get_data(obj0, tier0_content, content.length(), tier0_id);

    REQUIRE(tier0_content == content);

    uint8_t tier1_id = 1;
    copy(obj0, tier0_id, tier1_id);

    // std::cout << m_kv_store_client->dump() << std::endl;
    // std::cout << m_object_store_client->dump() << std::endl;

    /*
    REQUIRE(is_object_on_tier(obj0, tier1_id));

    get_data(obj0, &stream, tier1_id);
    check_content(&stream, content);

    move(obj2, tier0_id, tier1_id);
    REQUIRE_FALSE(is_object_on_tier(obj2, tier0_id));

    get_data(obj2, &stream, tier1_id);
    check_content(&stream, content);

    // Test list_objects()
    std::vector<hestia::Uuid> obj_ids;
    list_objects(obj_ids, tier0_id);
    REQUIRE(obj_ids.size() == 2);
    REQUIRE((obj_ids[0] == obj0.id() || obj_ids[0] == obj1.id()));
    REQUIRE((obj_ids[1] == obj0.id() || obj_ids[1] == obj1.id()));

    // Test remove()
    remove(obj1, tier0_id);
    REQUIRE_FALSE(is_object_on_tier(obj1, tier0_id));
    remove(obj2, tier1_id);
    REQUIRE_FALSE(is_object_on_tier(obj2, tier1_id));

    */
    // Test list_tiers()
    /*
    std::vector<uint8_t> tier_ids;
    list_tiers(obj0, tier_ids);
    REQUIRE(tier_ids.size() == 2);
    REQUIRE((tier_ids[0] == 0 || tier_ids[1] == 0));
    REQUIRE((tier_ids[0] == 1 || tier_ids[1] == 1));

    // Test list_attributes()
    std::string attrs;
    list_attributes(obj0, attrs);
    // TODO:check

    // Test removeall
    remove_all(obj0);
    REQUIRE_FALSE(is_object_on_tier(obj0, tier0_id));
    REQUIRE_FALSE(is_object_on_tier(obj0, tier1_id));
    */
}
