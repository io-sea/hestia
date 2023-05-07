#pragma once

#include "Metadata.h"

#include <ctime>
#include <string>

namespace hestia {
class S3Container {
  public:
    S3Container(const std::string& name = {}) : m_name(name)
    {
        m_creation_time = std::to_string(std::time(nullptr));
    }

    bool operator==(const S3Container& other) const
    {
        return m_name == other.m_name
               && m_creation_time == other.m_creation_time
               && m_user_metadata == other.m_user_metadata;
    }

    std::string m_name;
    std::string m_creation_time;
    Metadata m_user_metadata;
};
}  // namespace hestia