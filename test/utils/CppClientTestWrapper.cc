#include "CppClientTestWrapper.h"

#include <catch2/catch_all.hpp>

#include "ApplicationContext.h"
#include "HestiaConfigurator.h"

#include "FileUtils.h"
#include "TestUtils.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

void CppClientTestWrapper::init(
    const std::string& test_name, const std::string& config_name)
{
    m_test_name           = test_name;
    const auto cache_path = get_cache_path();
    hestia::FileUtils::empty_path(cache_path);

    const auto base_config_path =
        TestUtils::get_test_data_dir() / "configs" / config_name;
    REQUIRE(std::filesystem::is_regular_file(base_config_path));

    const auto test_config = cache_path + config_name;

    std::filesystem::copy_file(base_config_path, test_config);

    std::stringstream sstr;
    sstr << "\ncache_location: " << cache_path << "\n";
    {
        std::ofstream out_file(test_config, std::ios_base::app);
        out_file << sstr.str();
    }

    auto rc = hestia::initialize(test_config.c_str());
    REQUIRE(rc == 0);
}

CppClientTestWrapper::~CppClientTestWrapper()
{
    hestia::finish();
    std::filesystem::remove_all(get_cache_path());
}

std::string CppClientTestWrapper::get_cache_path() const
{
    return TestUtils::get_test_output_dir() / (m_test_name + "/");
}

void CppClientTestWrapper::get_and_check(
    const hestia::hsm_uint obj_id, uint8_t tier, const std::string& content)
{
    std::vector<char> return_buffer(content.length());
    auto rc =
        hestia::get(obj_id, return_buffer.data(), tier, content.length(), 0);
    REQUIRE(rc == 0);

    std::string returned_content(return_buffer.begin(), return_buffer.end());
    REQUIRE(returned_content == content);
}