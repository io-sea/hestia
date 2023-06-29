#include <string>

#ifdef TEST_REDIS

#include <catch2/catch_all.hpp>

#include "RedisKeyValueStoreClient.h"

#include "ApplicationContext.h"
#include "FileUtils.h"
#include "HestiaConfigurator.h"
#include "TestUtils.h"
#include "hestia.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

class HestiaRedisTestFixture {
  public:
    void init(const std::string& test_name)
    {
        m_test_name           = test_name;
        const auto cache_path = get_cache_path();
        hestia::FileUtils::empty_path(cache_path);

        const auto base_config_path = TestUtils::get_test_data_dir() / "configs"
                                      / "hestia_redis_tests.yaml";
        REQUIRE(std::filesystem::is_regular_file(base_config_path));

        const auto test_config = cache_path + "/hestia_redis_tests.yaml";

        std::filesystem::copy_file(base_config_path, test_config);

        std::stringstream sstr;
        sstr << "\ncache_location: " << cache_path << "\n";
        {
            std::ofstream out_file(test_config, std::ios_base::app);
            out_file << sstr.str();
        }

        const auto rc = hestia::initialize(test_config.c_str());
        REQUIRE(rc == 0);
    }

    ~HestiaRedisTestFixture()
    {
        hestia::finish();
        std::filesystem::remove_all(get_cache_path());
    }

    std::string get_cache_path() const
    {
        return TestUtils::get_test_output_dir(__FILE__) / m_test_name;
    }

    void get_and_check(
        const hestia::hsm_uint obj_id, uint8_t tier, const std::string& content)
    {
        std::vector<char> return_buffer(content.length());
        auto rc = hestia::get(
            obj_id, return_buffer.data(), tier, content.length(), 0);
        REQUIRE(rc == 0);

        std::string returned_content(
            return_buffer.begin(), return_buffer.end());
        REQUIRE(returned_content == content);
    }

    std::string m_test_name;
};


TEST_CASE_METHOD(
    HestiaRedisTestFixture, "Test Redis KV store backend", "[integration]")
{
    init("TestHestiaRedisKeyValueClient");

    hestia::hsm_uint obj_id{0000, 0001};

    std::string content = "The quick brown fox jumps over the lazy dog.";

    auto put_rc =
        hestia::put(obj_id, false, content.data(), 0, content.length(), 0);
    REQUIRE(put_rc == 0);

    get_and_check(obj_id, 0, content);

    auto copy_rc = hestia::copy(obj_id, 0, 1);
    REQUIRE(copy_rc == 0);

    get_and_check(obj_id, 1, content);

    auto move_rc = hestia::move(obj_id, 1, 2);
    REQUIRE(move_rc == 0);

    get_and_check(obj_id, 2, content);
}
#endif
