#include <catch2/catch_all.hpp>

#include "BasicDataPlacementEngine.h"
#include "DataPlacementEngine.h"
#include "FileKeyValueStoreClient.h"
#include "HsmKeyValueStore.h"
#include "HsmObjectStoreClientManager.h"
#include "HsmObjectStoreClientSpec.h"
#include "HsmService.h"
#include "HsmStoreInterface.h"
#include "MultiBackendHsmObjectStoreClient.h"
#include "TestUtils.h"

#include <iostream>

class TestHsmService : public hestia::HsmService {
  public:
    TestHsmService(
        std::unique_ptr<hestia::HsmKeyValueStore> kv_store,
        std::unique_ptr<hestia::MultiBackendHsmObjectStoreClient> object_client,
        std::unique_ptr<DataPlacementEngine> placement_engine) :
        hestia::HsmService(
            std::move(kv_store),
            std::move(object_client),
            std::move(placement_engine)){};

    hestia::HsmStoreInterface* get_store() { return m_store.get(); }
};

class HsmServiceTestFixture {
  public:
    ~HsmServiceTestFixture() { std::filesystem::remove_all(get_store_path()); }
    void put(const hestia::StorageObject& obj, hestia::Stream* stream)
    {
        hestia::HsmServiceRequest request(
            obj, hestia::HsmServiceRequestMethod::PUT);
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
        auto response = m_hsm_service->make_request(request);
        if (!(response->ok())) {
            std::cout << "EXISTS NOT OK!" << std::endl;
            return false;
        }
        else {
            return true;
        }
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

    bool obj_exists(const hestia::StorageObject& obj)
    {
        auto exists = m_hsm_service->get_store()->exists(obj);
        if (!exists->ok()) {
            return false;
        }
        else {
            return exists->object_found();
        }
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

    void init(const std::string& test_name)
    {
        m_test_name           = test_name;
        const auto store_path = get_store_path();
        std::filesystem::remove_all(store_path);
        hestia::Metadata config;
        config.set_item("root", store_path);

        auto kv_client = std::make_unique<hestia::FileKeyValueStoreClient>();
        kv_client->initialize(config);
        auto kv_store =
            std::make_unique<hestia::HsmKeyValueStore>(std::move(kv_client));

        auto client_registry =
            std::make_unique<hestia::HsmObjectStoreClientRegistry>(nullptr);
        auto client_manager =
            std::make_unique<hestia::HsmObjectStoreClientManager>(
                std::move(client_registry));
        auto object_client =
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

        object_client->do_initialize(g_all_file_backend_example, {});

        auto placement_engine = std::make_unique<BasicDataPlacementEngine>();

        m_hsm_service = std::make_unique<TestHsmService>(
            std::move(kv_store), std::move(object_client),
            std::move(placement_engine));
    }

    std::string get_store_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    std::string m_test_name;
    std::unique_ptr<TestHsmService> m_hsm_service;
};

TEST_CASE_METHOD(HsmServiceTestFixture, "HSM Service test", "[hsm-service]")
{
    init("TestHsmService");

    // Test put()
    hestia::StorageObject obj("0000");
    hestia::Stream stream;
    int src_tier = 0;

    put(obj, &stream);
    const std::string content = "The quick brown fox jumps over the lazy dog.";
    hestia::ReadableBufferView read_buffer(content);
    REQUIRE(stream.write(read_buffer).ok());
    REQUIRE(stream.reset().ok());
    REQUIRE(obj_exists(obj));

    // Test get()
    get(obj, &stream, src_tier);
    check_content(&stream, content);

    // Test copy() and move()
    int tgt_tier = 1;
    copy(obj, src_tier, tgt_tier);
    get(obj, &stream, src_tier);
    check_content(&stream, content);
    get(obj, &stream, tgt_tier);
    check_content(&stream, content);

    move(obj, src_tier, tgt_tier);
    REQUIRE(exists(obj, tgt_tier));
    get(obj, &stream, tgt_tier);
    check_content(&stream, content);
}
