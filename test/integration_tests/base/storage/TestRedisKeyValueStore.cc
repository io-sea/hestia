#include "Uuid.h"
#include <catch2/catch_test_macros.hpp>
#ifdef TEST_REDIS

#include <catch2/catch_all.hpp>

#include "RedisKeyValueStoreClient.h"

#include <string>

TEST_CASE("Test Redis KV store backend", "[integration]")
{
    hestia::RedisKeyValueStoreClient kv_store;

    hestia::RedisKeyValueStoreClientConfig config;
    config.m_redis_backend_address = "localhost";
    kv_store.do_initialize(config);

    // String Tests
    std::string sample_key1 = "hestia:object_name:object1";
    std::string sample_key2 = "hestia:object_name:object2";
    std::string json_value =
        "{\"acl\":{\"group\":[],\"user\":[]},\"created_by\":{\"creation_time\":\"1688558849220062831\",\"display_name\":\"\",\"id\":\"00000000-0000-0000-0000-000000000000\",\"is_admin\":\"false\",\"last_modified_time\":\"1688558849220062831\",\"name\":\"\",\"password\":\"\",\"token\":{\"created\":\"0\",\"value\":\"\"},\"type\":\"user\"}}";
    std::string uuid_string = "ffffffff-ffff-ffc8-ffff-ffffffffffff";
    std::vector<std::string> return_values;
    std::string return_value;

    // Set test
    REQUIRE_NOTHROW(kv_store.string_set(sample_key1, json_value));
    REQUIRE_NOTHROW(kv_store.string_set(sample_key2, uuid_string));

    // Existence test
    bool exists;
    REQUIRE_NOTHROW(exists = kv_store.string_exists(sample_key1));
    REQUIRE(exists);

    // Get test
    REQUIRE_NOTHROW(kv_store.string_get(sample_key1, return_value));
    REQUIRE(return_value == json_value);
    REQUIRE_NOTHROW(kv_store.string_get(sample_key2, return_value));
    REQUIRE(return_value == uuid_string);

    // Multi-Get Test
    REQUIRE_NOTHROW(
        kv_store.string_multi_get({sample_key1, sample_key2}, return_values));
    REQUIRE(return_values.size() == 2);
    REQUIRE(return_values[0] == json_value);
    REQUIRE(return_values[1] == uuid_string);

    // Update test
    REQUIRE_NOTHROW(kv_store.string_set(sample_key1, uuid_string));
    REQUIRE_NOTHROW(kv_store.string_get(sample_key1, return_value));
    REQUIRE(return_value == uuid_string);

    // Delete test
    REQUIRE_NOTHROW(kv_store.string_remove(sample_key1));
    REQUIRE_NOTHROW(kv_store.string_remove(sample_key2));

    REQUIRE_NOTHROW(exists = kv_store.string_exists(sample_key1));
    REQUIRE_FALSE(exists);
    REQUIRE_NOTHROW(exists = kv_store.string_exists(sample_key2));
    REQUIRE_FALSE(exists);

    // Set tests
    hestia::Uuid uuid = {0000, 0001};
    std::vector<hestia::Uuid> return_values_uuid;

    REQUIRE_NOTHROW(kv_store.set_add(sample_key1, uuid));

    REQUIRE_NOTHROW(exists = kv_store.string_exists(sample_key1));
    REQUIRE(exists);

    REQUIRE_NOTHROW(kv_store.set_list(sample_key1, return_values_uuid));
    REQUIRE(return_values_uuid.size() == 1);
    REQUIRE(return_values_uuid[0] == uuid);

    return_values_uuid.clear();
    REQUIRE_NOTHROW(kv_store.set_remove(sample_key1, uuid));
    REQUIRE_NOTHROW(kv_store.set_list(sample_key1, return_values_uuid));
    REQUIRE(return_values_uuid.empty());

    REQUIRE_NOTHROW(kv_store.string_remove(sample_key1));
    REQUIRE_NOTHROW(exists = kv_store.string_exists(sample_key1));
    REQUIRE_FALSE(exists);
}
#endif
