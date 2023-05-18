#pragma once

#include "TestDirectories.h"
#include <filesystem>

class TestUtils {
  public:
    static std::filesystem::path get_test_output_dir(
        const std::string& test_file_name = {})
    {
        if (!test_file_name.empty()) {
            const auto name =
                std::filesystem::path(test_file_name).filename().stem();
            return std::filesystem::current_path() / "test_output" / name;
        }
        else {
            return std::filesystem::current_path() / "test_output";
        }
    }

    static std::filesystem::path get_test_data_dir()
    {
        return TestDirectories::get_test_data_dir();
    }
};