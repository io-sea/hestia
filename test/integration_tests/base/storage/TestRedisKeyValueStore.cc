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
    config.m_backend_address.update_value("localhost");
    kv_store.do_initialize({}, config);

    // String Tests
    std::string sample_key1 = "hestia:object_name:object1";
    std::string sample_key2 = "hestia:object_name:object2";
    std::string json_value =
        "{\"acl\":{\"group\":[],\"user\":[]},\"created_by\":{\"creation_time\":\"1688558849220062831\",\"display_name\":\"\",\"id\":\"00000000-0000-0000-0000-000000000000\",\"is_admin\":\"false\",\"last_modified_time\":\"1688558849220062831\",\"name\":\"\",\"password\":\"\",\"token\":{\"created\":\"0\",\"value\":\"\"},\"type\":\"user\"}}";
    std::string uuid_string = "ffffffff-ffff-ffc8-ffff-ffffffffffff";

    // Set test
    REQUIRE_NOTHROW(kv_store.string_set({{sample_key1, json_value}}));
    REQUIRE_NOTHROW(kv_store.string_set({{sample_key2, uuid_string}}));

    // Existence test
    std::vector<bool> exists;
    REQUIRE_NOTHROW(kv_store.string_exists({sample_key1}, exists));
    REQUIRE(exists[0]);
    exists.clear();

    // Get test
    std::vector<std::string> return_value;
    REQUIRE_NOTHROW(kv_store.string_get({sample_key1}, return_value));
    REQUIRE(return_value[0] == json_value);
    return_value.clear();

    REQUIRE_NOTHROW(kv_store.string_get({sample_key2}, return_value));
    REQUIRE(return_value[0] == uuid_string);
    return_value.clear();

    // Update test
    REQUIRE_NOTHROW(kv_store.string_set({{sample_key1, uuid_string}}));
    REQUIRE_NOTHROW(kv_store.string_get({sample_key1}, return_value));
    REQUIRE(return_value[0] == uuid_string);
    return_value.clear();

    // Delete test
    REQUIRE_NOTHROW(kv_store.string_remove({sample_key1}));
    REQUIRE_NOTHROW(kv_store.string_remove({sample_key2}));

    REQUIRE_NOTHROW(kv_store.string_exists({sample_key1}, exists));
    REQUIRE_FALSE(exists[0]);
    exists.clear();

    REQUIRE_NOTHROW(kv_store.string_exists({sample_key2}, exists));
    REQUIRE_FALSE(exists[0]);
    exists.clear();

    // Set tests
    std::string id{"0000-1111"};
    std::vector<std::vector<std::string>> return_values_uuid;

    REQUIRE_NOTHROW(kv_store.set_add({{sample_key1, id}}));

    REQUIRE_NOTHROW(kv_store.set_list({sample_key1}, return_values_uuid));

    REQUIRE(return_values_uuid.size() == 1);
    REQUIRE(return_values_uuid[0].size() == 1);
    REQUIRE(return_values_uuid[0][0] == id);

    return_values_uuid.clear();

    REQUIRE_NOTHROW(kv_store.set_remove({{sample_key1, id}}));
    REQUIRE_NOTHROW(kv_store.set_list({sample_key1}, return_values_uuid));
    REQUIRE(return_values_uuid[0].empty());
}
#endif
