#pragma once

#include <filesystem>
#include <string>

class CacheTestFixture {
  public:
    virtual void do_init(
        const std::string& test_file, const std::string& test_name);

    virtual ~CacheTestFixture();

  protected:
    std::string get_cache_path() const;
    std::filesystem::path m_test_file;
    std::string m_config_path;
    std::string m_test_name;
};