#include <catch2/catch_all.hpp>

#include "ApplicationContext.h"
#include "EventFeed.h"
#include "HestiaConfigurator.h"
#include "hestia.h"

#include "Dictionary.h"
#include "FileUtils.h"
#include "TestUtils.h"
#include "YamlUtils.h"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class EventFeedTestFixture {
  public:
    void init(const std::string& test_name)
    {
        m_test_name = test_name;

        const auto output_dir = get_output_dir();
        hestia::FileUtils::empty_path(output_dir);

        // Hestia Init

        // Get hestia config
        const auto base_config_path =
            TestUtils::get_test_data_dir() / "configs" / "defaults.yaml";
        REQUIRE(std::filesystem::is_regular_file(base_config_path));

        const auto test_config = output_dir + "/test_config.yaml";
        std::filesystem::copy_file(base_config_path, test_config);

        std::stringstream sstr;
        sstr << "\ncache_location: " << output_dir << "\n";
        sstr << "\nevent_feed:"
             << "\n  event_feed_path: " << get_output_path()
             << "\n  event_feed_sorted: y\n";
        {
            std::ofstream out_file(test_config, std::ios_base::app);
            out_file << sstr.str();
        }

        const auto rc = hestia::initialize(test_config.c_str());
        REQUIRE(rc == 0);
    }

    ~EventFeedTestFixture()
    {
        hestia::finish();
        // std::filesystem::remove_all(get_output_dir());
    }

    std::string get_output_dir() const
    {
        return TestUtils::get_test_output_dir() / "event_feed";
    }

    std::string get_output_path() const
    {
        return TestUtils::get_test_output_dir() / "event_feed"
               / ("test_event_feed_" + m_test_name + ".yaml");
    }

    std::string get_ref_path() const
    {
        return TestUtils::get_test_data_dir() / "event_feed"
               / ("test_event_feed_" + m_test_name + ".yaml");
    }

    void check_output()
    {
        std::ifstream ref_file(get_ref_path()), test_file(get_output_path());
        std::string ref_yaml(
            (std::istreambuf_iterator<char>(ref_file)),
            (std::istreambuf_iterator<char>()));
        std::string test_yaml(
            (std::istreambuf_iterator<char>(test_file)),
            (std::istreambuf_iterator<char>()));

        REQUIRE(ref_yaml == test_yaml);

        ref_file.close();
        test_file.close();
    }

    std::string m_test_name;
};

TEST_CASE_METHOD(
    EventFeedTestFixture,
    "Test empty event feed output validity",
    "[event-feed]")
{
    init("empty");
    // Do nothing
    check_output();
}

TEST_CASE_METHOD(
    EventFeedTestFixture,
    "Test event feed output for Hestia methods",
    "[event-feed]")
{
    init("all_methods");

    auto obj_id = hestia::hsm_uint{0000, 0001};

    std::string content = "The quick brown fox jumps over the lazy dog.";

    auto put_rc =
        hestia::put(obj_id, false, content.data(), 0, content.length(), 0);
    REQUIRE(put_rc == 0);

    auto copy_rc = hestia::copy(obj_id, 0, 1);
    REQUIRE(copy_rc == 0);

    auto move_rc = hestia::move(obj_id, 1, 2);
    REQUIRE(move_rc == 0);

    // TODO: Release
    // auto release_rc = hestia::remove(obj_id, 0);
    // REQUIRE(release_rc == 0);

    // TODO: Remove
    // auto remove_rc = hestia::remove(obj_id);
    // REQUIRE(remove_rc == 0);

    // TODO: Set-attrs
    // auto set_attrs_rc = hestia::set_attrs(obj_id, "test: test");
    // REQUIRE(set_attrs_rc == 0);

    check_output();
}