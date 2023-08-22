#include <catch2/catch_all.hpp>

#include "EventFeed.h"
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
        hestia::FileUtils::empty_directory(output_dir);
    }

    ~EventFeedTestFixture()
    {
        // std::filesystem::remove_all(get_output_dir());
    }

    std::string get_output_dir() const
    {
        return TestUtils::get_test_output_dir() / "TestEventFeed";
    }

    std::string get_output_path() const
    {
        return TestUtils::get_test_output_dir() / "TestEventFeed"
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

    /*
    std::vector<char> buffer(HESTIA_UUID_SIZE);
    auto rc = hestia_object_create(buffer.data());
    std::string id(buffer.begin(), buffer.end());

    std::string content = "The quick brown fox jumps over the lazy dog.";
    rc = hestia_object_put(id.c_str(), content.data(), 0, content.length(), 0);
    REQUIRE(rc == 0);

    rc = hestia_object_copy(id.c_str(), 0, 1);
    REQUIRE(rc == 0);

    rc = hestia_object_move(id.c_str(), 1, 2);
    REQUIRE(rc == 0);


    // TODO: Release
    // auto release_rc = hestia::remove(obj_id, 0);
    // REQUIRE(release_rc == 0);

    // TODO: Remove
    // auto remove_rc = hestia::remove(obj_id);
    // REQUIRE(remove_rc == 0);

    // TODO: Set-attrs
    // auto set_attrs_rc = hestia::set_attrs(obj_id, "test: test");
    // REQUIRE(set_attrs_rc == 0);

    // check_output();
    */
}
