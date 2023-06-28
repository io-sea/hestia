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
    //  ~HsmServiceTestFixture() {
    //  std::filesystem::remove_all(get_store_path()); }

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

    void put(
        const hestia::StorageObject& obj, hestia::Stream* stream, uint8_t tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::PUT);
        request.set_target_tier(tier);
        REQUIRE(m_hsm_service->make_request(request, stream)->ok());
    }

    void get(
        const hestia::StorageObject& obj, hestia::Stream* stream, uint8_t tier)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::GET);
        request.set_source_tier(tier);
        REQUIRE(m_hsm_service->make_request(request, stream)->ok());
    }

    bool exists(const hestia::StorageObject& obj)
    {
        auto exists = m_hsm_service->make_request(
            {obj, hestia::HsmServiceRequestMethod::EXISTS});
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
        REQUIRE(m_hsm_service->make_request(request)->ok());
    }

    void remove_all(const hestia::StorageObject& obj)
    {
        REQUIRE(m_hsm_service
                    ->make_request(
                        {obj, hestia::HsmServiceRequestMethod::REMOVE_ALL})
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

    void list_tiers(const hestia::StorageObject& obj, std::vector<uint8_t>& ids)
    {
        auto response = m_hsm_service->make_request(
            {obj, hestia::HsmServiceRequestMethod::LIST_TIERS});
        REQUIRE(response->ok());

        for (const auto& tier_id : response->tiers()) {
            ids.push_back(tier_id.id_uint());
        }
    }

    void list_attributes(const hestia::StorageObject& obj, std::string& attrs)
    {
        auto response = m_hsm_service->make_request(
            {obj, hestia::HsmServiceRequestMethod::LIST_ATTRIBUTES});
        REQUIRE(response->ok());
        attrs = "test metadata";
    }

    bool is_object_on_tier(const hestia::StorageObject& obj, int tier)
    {
        auto response = m_hsm_service->make_request(
            {obj, hestia::HsmServiceRequestMethod::LIST_TIERS});

        for (const auto& tier_id : response->tiers()) {
            if (tier_id.id_uint() == tier) {
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
    hestia::Uuid id0{0000};
    hestia::StorageObject obj0(id0);

    hestia::Stream stream;
    uint8_t tier0_id = 0;

    const std::string content = "The quick brown fox jumps over the lazy dog.";
    hestia::ReadableBufferView read_buffer(content);

    obj0.m_size = content.length();
    put(obj0, &stream, tier0_id);
    REQUIRE(stream.write(read_buffer).ok());
    REQUIRE(stream.reset().ok());

    REQUIRE(exists(obj0));

    hestia::Uuid id1{0001};
    hestia::Uuid id2{0002};
    hestia::StorageObject obj1(id1);
    hestia::StorageObject obj2(id2);
    obj1.m_size = content.length();
    put(obj1, &stream, tier0_id);
    REQUIRE(stream.write(read_buffer).ok());
    REQUIRE(stream.reset().ok());
    REQUIRE(exists(obj1));

    obj2.m_size = content.length();
    put(obj2, &stream, tier0_id);
    REQUIRE(stream.write(read_buffer).ok());
    REQUIRE(stream.reset().ok());
    REQUIRE(exists(obj2));

    // Test get()
    get(obj0, &stream, tier0_id);
    check_content(&stream, content);

    uint8_t tier1_id = 1;
    copy(obj0, tier0_id, tier1_id);
    REQUIRE(is_object_on_tier(obj0, tier1_id));

    get(obj0, &stream, tier1_id);
    check_content(&stream, content);

    move(obj2, tier0_id, tier1_id);
    REQUIRE_FALSE(is_object_on_tier(obj2, tier0_id));

    get(obj2, &stream, tier1_id);
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

    // Test list_tiers()
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
}
