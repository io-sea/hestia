#pragma once

#include <string>

namespace hestia {
class S3Container {
  public:
    S3Container(const std::string& name = {}) : m_name(name) {}

    bool operator==(const S3Container& other) const
    {
        return m_name == other.m_name;
    }

    std::string m_name;
};
}  // namespace hestia