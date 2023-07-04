#pragma once

#include "hestia.h"

#include <string>

class CppClientTestWrapper {
  public:
    void init(
        const std::string& test_name,
        const std::string& config_name = "defaults.yaml");

    ~CppClientTestWrapper();

    std::string get_cache_path() const;

    void get_and_check(
        const hestia::hsm_uint obj_id,
        uint8_t tier,
        const std::string& content);

  private:
    std::string m_test_name;
};
