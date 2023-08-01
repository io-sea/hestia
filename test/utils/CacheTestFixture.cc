#include "CacheTestFixture.h"

#include "FileUtils.h"

#include "TestUtils.h"

#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

void CacheTestFixture::do_init(
    const std::string& test_file, const std::string& test_name)
{
    m_test_name = test_name;
    m_test_file = test_file;

    const auto cache_path = get_cache_path();
    hestia::FileUtils::empty_path(cache_path);

    const auto base_config_path =
        TestUtils::get_test_data_dir() / "configs" / "defaults.yaml";
    REQUIRE(std::filesystem::is_regular_file(base_config_path));

    m_config_path = cache_path + "/defaults.yaml";

    std::filesystem::copy_file(base_config_path, m_config_path);

    std::stringstream sstr;
    sstr << "\ncache_path: " << cache_path << "\n";
    {
        std::ofstream out_file(m_config_path, std::ios_base::app);
        out_file << sstr.str();
    }
}

CacheTestFixture::~CacheTestFixture()
{
    std::filesystem::remove_all(get_cache_path());
}

std::string CacheTestFixture::get_cache_path() const
{
    return TestUtils::get_test_output_dir(m_test_file) / m_test_name;
}