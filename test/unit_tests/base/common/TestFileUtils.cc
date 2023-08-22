#include <catch2/catch_all.hpp>

#include "FileUtils.h"
#include "TestUtils.h"

#include <filesystem>
#include <fstream>

TEST_CASE("Test FileUtils - file name/extension", "[common]")
{
    auto test_output_dir = TestUtils::get_test_output_dir();
    std::filesystem::current_path(TestUtils::get_test_output_dir());
    std::filesystem::directory_entry dir_entry{"fileutils_dir"};
    std::filesystem::create_directory(dir_entry);
    const std::filesystem::path path{dir_entry};


    std::ofstream(path / "file.txt");
    std::filesystem::directory_entry file_path{path / "file.txt"};
    REQUIRE(hestia::FileUtils::is_file_with_extension(file_path, ".txt"));
    REQUIRE(
        hestia::FileUtils::get_filename_without_extension(file_path) == "file");

    std::ofstream(path / "new_file.txt");
    std::filesystem::directory_entry new_path{path / "new_file.txt"};
    hestia::FileUtils::create_if_not_existing(new_path);
    REQUIRE(
        hestia::FileUtils::get_filename_without_extension(new_path)
        == "new_file");

    REQUIRE(hestia::FileUtils::get_file_size(file_path) == 0);

    hestia::FileUtils::empty_directory(dir_entry);
    REQUIRE(is_empty(dir_entry));

    std::filesystem::remove_all(dir_entry);
}
