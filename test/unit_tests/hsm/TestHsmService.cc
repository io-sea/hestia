#include <catch2/catch_all.hpp>

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "FileHsmObjectStoreClient.h"
#include "FileKeyValueStoreClient.h"

#include "DatasetService.h"
#include "HsmService.h"
#include "ObjectService.h"
#include "TierService.h"

#include "TestUtils.h"

#include <iostream>


class TestHsmService : public hestia::HsmService {
  public:
    TestHsmService(
        std::unique_ptr<hestia::ObjectService> object_service,
        std::unique_ptr<hestia::TierService> tier_service,
        std::unique_ptr<hestia::DatasetService> dataset_service,
        hestia::HsmObjectStoreClient* object_store,
        std::unique_ptr<hestia::DataPlacementEngine> placement_engine) :
        hestia::HsmService(
            std::move(object_service),
            std::move(tier_service),
            std::move(dataset_service),
            object_store,
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

        auto dataset_service = hestia::DatasetService::create(
            hestia::DatasetServiceConfig(), m_kv_store_client.get());

        for (std::size_t idx = 0; idx < 5; idx++) {
            auto response = tier_service->make_request(
                {hestia::StorageTier(idx), hestia::CrudMethod::PUT});
            REQUIRE(response->ok());
        }

        m_object_store_client =
            std::make_unique<hestia::FileHsmObjectStoreClient>();
        hestia::FileHsmObjectStoreClientConfig file_config;
        file_config.m_root = get_store_path();
        m_object_store_client->do_initialize(file_config);

        auto placement_engine =
            std::make_unique<hestia::BasicDataPlacementEngine>(
                tier_service.get());

        m_hsm_service = std::make_unique<TestHsmService>(
            std::move(object_service), std::move(tier_service),
            std::move(dataset_service), m_object_store_client.get(),
            std::move(placement_engine));
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

    bool exists(const hestia::StorageObject& obj)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::EXISTS);
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

    void remove(const hestia::StorageObject& obj, int tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::REMOVE);
        request.set_source_tier(tier);
        request.set_target_tier(tier);
        REQUIRE(m_hsm_service->make_request(request)->ok());
    }

    void remove_all(const hestia::StorageObject& obj)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::REMOVE_ALL);
        REQUIRE(m_hsm_service->make_request(request)->ok());
    }

    void list_objects(std::vector<std::string>& ids, uint8_t tier)
    {
        hestia::HsmServiceRequest request(
            tier, hestia::HsmServiceRequestMethod::LIST);
        request.set_source_tier(tier);
        request.tier()=tier;
        auto response = m_hsm_service->make_request(request);
        REQUIRE(response->ok());
        for(const auto&  object_id: response->objects()){
          ids.push_back(object_id.id());
        }
    }

    void list_tiers(const hestia::StorageObject& obj, std::vector<std::string>& ids)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::LIST_TIERS);
        auto response = m_hsm_service->make_request(request);
        REQUIRE(response->ok());

        for(const auto&  tier_id: response->tiers()){
          ids.push_back(tier_id.id());
        }
    }

    void list_attributes(const hestia::StorageObject& obj, std::string& attrs)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::LIST_ATTRIBUTES);
        auto response = m_hsm_service->make_request(request);
        std::cout<<response->ok()<<std::endl;
        REQUIRE(response->ok());
        attrs = "test metadata";

        std::cout<<"metadata: "<< response->object().metadata() <<std::endl;
    }

    bool is_object_on_tier(const hestia::StorageObject& obj, int tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::LIST_TIERS);
        auto response = m_hsm_service->make_request(request);

        for(const auto&  tier_id: response->tiers()) {
            if(tier_id.id_uint() == tier) {
            return true;
            }
        }
        return false;
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
    std::unique_ptr<hestia::FileHsmObjectStoreClient> m_object_store_client;
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
    std::string attrs;

    put(obj0, &stream0, src_tier);
    put(obj1, &stream1, src_tier);
    put(obj2, &stream2, src_tier);

    const std::string content = "The quick brown fox jumps over the lazy dog.";
    hestia::ReadableBufferView read_buffer(content);
    REQUIRE(stream0.write(read_buffer).ok());
    REQUIRE(stream0.reset().ok());
    REQUIRE(exists(obj0));
    REQUIRE(stream1.write(read_buffer).ok());
    REQUIRE(stream1.reset().ok());
    REQUIRE(exists(obj1));
    REQUIRE(stream2.write(read_buffer).ok());
    REQUIRE(stream2.reset().ok());
    REQUIRE(exists(obj2));

    // Test get()
    get(obj0, &stream0, src_tier);
    check_content(&stream0, content);

    // Test copy() and move()
    copy(obj0, src_tier, tgt_tier);
    REQUIRE(is_object_on_tier(obj0, tgt_tier));
    get(obj0, &stream0, tgt_tier);
    check_content(&stream0, content);

    move(obj2, src_tier, tgt_tier);
    REQUIRE_FALSE(is_object_on_tier(obj2, src_tier));
    get(obj2, &stream2, tgt_tier);
    check_content(&stream2, content);

    //Test list_objects()
    list_objects(obj_ids, src_tier);
    REQUIRE(obj_ids.size() == 2);
    REQUIRE(obj_ids[0] == obj0.id());
    REQUIRE(obj_ids[1] == obj1.id());

    //Test remove()
    remove(obj1, src_tier);
    REQUIRE_FALSE(is_object_on_tier(obj1, src_tier));
    remove(obj2, tgt_tier);
    REQUIRE_FALSE(is_object_on_tier(obj2, tgt_tier));

    //Test list_tiers()
    list_tiers(obj0, tier_ids);
    REQUIRE(tier_ids.size() == 2);
    REQUIRE(tier_ids[0] == "0");
    REQUIRE(tier_ids[1] == "1");

    //Test list_attributes()
    list_attributes(obj0, attrs);
    //TODO:check

    //Test removeall
    remove_all(obj0);
    REQUIRE_FALSE(is_object_on_tier(obj0, src_tier));
    REQUIRE_FALSE(is_object_on_tier(obj0, tgt_tier));
}
