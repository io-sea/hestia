#include <catch2/catch_all.hpp>

#include "File.h"
#include "FileUtils.h"
#include "TestUtils.h"

#include <filesystem>
#include <fstream>

TEST_CASE("Test FileUtils - file name/extension", "[common]")
{
    auto test_output_dir = TestUtils::get_test_output_dir() / "TestFileUtils";

    std::filesystem::create_directory(test_output_dir);

    std::ofstream(test_output_dir / "file.txt");
    std::filesystem::directory_entry file_path{test_output_dir / "file.txt"};
    REQUIRE(hestia::FileUtils::is_file_with_extension(file_path, ".txt"));
    REQUIRE(
        hestia::FileUtils::get_filename_without_extension(file_path) == "file");

    std::ofstream(test_output_dir / "new_file.txt");
    std::filesystem::directory_entry new_path{test_output_dir / "new_file.txt"};
    hestia::FileUtils::create_if_not_existing(new_path);
    REQUIRE(
        hestia::FileUtils::get_filename_without_extension(new_path)
        == "new_file");

    REQUIRE(hestia::FileUtils::get_file_size(file_path) == 0);

    hestia::FileUtils::empty_directory(test_output_dir);
    REQUIRE(is_empty(test_output_dir));

    std::filesystem::remove_all(test_output_dir);
}

TEST_CASE("Test File- read/write", "[common]")
{
    std::string test_data = "Testing data 123";

    // auto test_output_dir = TestUtils::get_test_output_dir();
    // std::filesystem::current_path(test_output_dir);

    const std::filesystem::path testpath{std::filesystem::current_path()};

    hestia::File testfile(testpath / "testfile.txt");

    REQUIRE(testfile.write(test_data.data(), test_data.size()).ok());

    testfile.close();

    std::string read_buffer;
    read_buffer.resize(test_data.size());
    hestia::File readfile(testpath / "testfile.txt");

    auto result = readfile.read(read_buffer.data(), read_buffer.size());
    REQUIRE(result.first.ok());

    REQUIRE(test_data == read_buffer);

    std::size_t offset = 8;

    std::string read_buffer2;
    read_buffer2.resize(test_data.size() - offset);

    readfile.seek_to(offset);
    readfile.read(read_buffer2.data(), read_buffer2.size());

    auto offset_test_data = test_data.substr(offset);

    REQUIRE(offset_test_data == read_buffer2);

    std::string test_2 = "Testing data 456";
    std::string test_3 = "Testing data 789";

    std::vector<std::string> test_vector{test_data, test_2, test_3};

    hestia::File testfile2(testpath / "testfile2.txt");

    REQUIRE(testfile2.write_lines(test_vector).ok());
    testfile2.close();

    std::vector<std::string> read_vector{};
    hestia::File readfile2(testpath / "testfile2.txt");
    auto result2 = readfile2.read_lines(read_vector);

    readfile2.close();

    REQUIRE(result2.first.ok());
    REQUIRE(test_vector == read_vector);

    // testfile.close();
    // REQUIRE(!testfile.write(test_data.data(), test_data.size()).ok());
}
