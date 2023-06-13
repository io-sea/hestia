#include <catch2/catch_all.hpp>

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "FileKeyValueStoreClient.h"

#include "CopyToolInterface.h"
#include "HsmObjectStoreClientManager.h"
#include "HsmObjectStoreClientSpec.h"
#include "HsmService.h"
#include "MultiBackendHsmObjectStoreClient.h"
#include "ObjectService.h"
#include "TierService.h"

#include "TestUtils.h"

#include <iostream>

class TestHsmService : public hestia::HsmService {
  public:
    TestHsmService(
        std::unique_ptr<hestia::ObjectService> object_service,
        std::unique_ptr<hestia::TierService> tier_service,
        hestia::MultiBackendHsmObjectStoreClient* object_client,
        std::unique_ptr<hestia::DataPlacementEngine> placement_engine) :
        hestia::HsmService(
            std::move(object_service),
            std::move(tier_service),
            object_client,
            std::move(placement_engine)){};

    virtual ~TestHsmService() {}
};

class HsmServiceTestFixture {
  public:
  //  ~HsmServiceTestFixture() { std::filesystem::remove_all(get_store_path()); }

    void init(const std::string& test_name)
    {
        m_test_name           = test_name;
        const auto store_path = get_store_path();
        std::filesystem::remove_all(store_path);
        hestia::Metadata config;
        config.set_item("root", store_path);

        m_kv_store_client = std::make_unique<hestia::FileKeyValueStoreClient>();
        m_kv_store_client->initialize(config);

        hestia::ObjectServiceConfig object_config;
        auto object_service = hestia::ObjectService::create(
            object_config, m_kv_store_client.get());

        auto tier_service = hestia::TierService::create(
            hestia::TierServiceConfig(), m_kv_store_client.get());

        auto client_factory =
            std::make_unique<hestia::HsmObjectStoreClientFactory>(nullptr);
        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(
                std::move(client_factory));
        m_object_store_client =
            std::make_unique<hestia::MultiBackendHsmObjectStoreClient>(
                std::move(client_manager));

        hestia::HsmObjectStoreClientSpec my_spec(
            hestia::HsmObjectStoreClientSpec::Type::HSM,
            hestia::HsmObjectStoreClientSpec::Source::BUILT_IN,
            "hestia::FileHsmObjectStoreClient");
        my_spec.m_extra_config.set_item("root", get_store_path());
        hestia::TierBackendRegistry g_all_file_backend_example;
        g_all_file_backend_example.emplace(0, my_spec);
        g_all_file_backend_example.emplace(1, my_spec);

        m_object_store_client->do_initialize(g_all_file_backend_example, {});
        auto placement_engine =
            std::make_unique<hestia::BasicDataPlacementEngine>(
                tier_service.get());

        m_hsm_service = std::make_unique<TestHsmService>(
            std::move(object_service), std::move(tier_service),
            m_object_store_client.get(), std::move(placement_engine));
    }

    void put(const hestia::StorageObject& obj, hestia::Stream* stream, uint8_t tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::PUT);
        request.set_source_tier(tier);
        request.set_target_tier(tier);
        REQUIRE(m_hsm_service->make_request(request, stream)->ok());
    }

    void get(
        const hestia::StorageObject& obj, hestia::Stream* stream, uint8_t tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::GET);
        request.set_source_tier(tier);
        request.set_target_tier(tier);
        REQUIRE(m_hsm_service->make_request(request, stream)->ok());
    }

    bool exists(const hestia::StorageObject& obj, uint8_t tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::EXISTS);
        request.set_source_tier(tier);
        request.set_target_tier(tier);
        auto exists = m_hsm_service->make_request(request);
        REQUIRE(exists->ok());
        return exists->object_found();
    }

    void copy(const hestia::StorageObject& obj, int src_tier, int tgt_tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::COPY);
        request.set_source_tier(src_tier);
        request.set_target_tier(tgt_tier);
        REQUIRE(m_hsm_service->make_request(request)->ok());
    }

    void move(const hestia::StorageObject& obj, int src_tier, int tgt_tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::MOVE);
        request.set_source_tier(src_tier);
        request.set_target_tier(tgt_tier);
        REQUIRE(m_hsm_service->make_request(request)->ok());
    }

    void remove(const hestia::StorageObject& obj, int src_tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::REMOVE);
        request.set_source_tier(src_tier);
        REQUIRE(m_hsm_service->make_request(request)->ok());
    }

    void remove_all(const hestia::StorageObject& obj)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::REMOVE_ALL);
        REQUIRE(m_hsm_service->make_request(request)->ok());
    }

    void list_objects(std::vector<std::string>& ids, int src_tier)
    {
        hestia::HsmServiceRequest request(
            src_tier, hestia::HsmServiceRequestMethod::LIST);
        request.set_source_tier(src_tier);
        auto response = m_hsm_service->make_request(request);
        REQUIRE(response->ok());
        //ids={"0000", "0001", "0002"};
        int index=0;
        for(auto i: response->objects()){
          ids[index]=i.id();
          index=index+1;
          std::cout<<"id (objects):"<<i.id()<<std::endl;
        }
        std::cout<<"index (objects):"<<index<<std::endl;
    }

    void list_tiers(const hestia::StorageObject& obj, std::vector<std::string>& ids)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::LIST_TIERS);
        auto response = m_hsm_service->make_request(request);
        REQUIRE(response->ok());
        //ids={"0", "1"};
        int index=0;
        for(auto i: response->tiers()){
          ids[index]=i.id();
          index=index+1;
          std::cout<<"id (tiers):"<<i.id()<<std::endl;
        }
        std::cout<<"index (tiers):"<<index<<std::endl;
    }

    void check_content(hestia::Stream* stream, const std::string& content)
    {
        std::vector<char> returned_buffer(content.length());
        hestia::WriteableBufferView write_buffer(returned_buffer);
        REQUIRE(stream->read(write_buffer).ok());
        REQUIRE(stream->reset().ok());

        std::string recontstructed_content(
            returned_buffer.begin(), returned_buffer.end());
        REQUIRE(recontstructed_content == content);
    }

    std::string get_store_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    std::string m_test_name;
    std::unique_ptr<hestia::KeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::MultiBackendHsmObjectStoreClient>
        m_object_store_client;
    std::unique_ptr<TestHsmService> m_hsm_service;
};

TEST_CASE_METHOD(HsmServiceTestFixture, "HSM Service test", "[hsm-service]")
{
    init("TestHsmService");

    // Test put()
    hestia::StorageObject obj0("0000"), obj1("0001"), obj2("0002");
    hestia::Stream stream0, stream1, stream2;
    int src_tier = 0;
    int tgt_tier = 1;
    std::vector<std::string> obj_ids, tier_ids;

    put(obj0, &stream0, src_tier);
    put(obj1, &stream1, src_tier);
    put(obj2, &stream2, src_tier);

    const std::string content = "The quick brown fox jumps over the lazy dog.";
    hestia::ReadableBufferView read_buffer(content);
    REQUIRE(stream0.write(read_buffer).ok());
    REQUIRE(stream0.reset().ok());
    REQUIRE(exists(obj0, src_tier));
    REQUIRE(stream1.write(read_buffer).ok());
    REQUIRE(stream1.reset().ok());
    REQUIRE(exists(obj1, src_tier));
    REQUIRE(stream2.write(read_buffer).ok());
    REQUIRE(stream2.reset().ok());
    REQUIRE(exists(obj2, src_tier));

    // Test get()
    get(obj0, &stream0, src_tier);
    check_content(&stream0, content);

    // Test copy() and move()
    copy(obj0, src_tier, tgt_tier);
    REQUIRE(exists(obj0, src_tier));
    REQUIRE(exists(obj0, tgt_tier));
    get(obj0, &stream0, tgt_tier);
    check_content(&stream0, content);

    //move(obj0, src_tier, tgt_tier);
    //REQUIRE_FALSE(exists(obj0, src_tier));
    //REQUIRE(exists(obj0, tgt_tier));
    //get(obj0, &stream0, tgt_tier);
    //check_content(&stream0, content);

    //Test remove
    //remove(obj0, tgt_tier);
    //REQUIRE_FALSE(exists(obj0, tgt_tier));

    //Test removeall
    //copy(obj1, src_tier, tgt_tier);
    //remove_all(obj1);
    //REQUIRE_FALSE(exists(obj1, src_tier));
    //REQUIRE_FALSE(exists(obj1, tgt_tier));

    //Test list_objects
    //list_objects(obj_ids, src_tier);
    //REQUIRE(obj_ids.size() == 3);
    //REQUIRE(obj_ids[0] == obj0.id());
    //REQUIRE(obj_ids[1] == obj1.id());
    //REQUIRE(obj_ids[2] == obj2.id());


    //Test list_tiers
    list_tiers(obj0, tier_ids);
    //REQUIRE(tier_ids.size() == 2);
    //REQUIRE(tier_ids[0] == 0);
    //REQUIRE(tier_ids[1] == 1);
}
