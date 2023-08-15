#pragma once

#include "Map.h"
#include <string>

namespace hestia {
class S3Object {
  public:
    S3Object(const std::string& name = {});

    bool is_equal(const S3Object& other) const;

    std::string to_string() const;

    bool operator==(const S3Object& other) const { return is_equal(other); }

    friend std::ostream& operator<<(std::ostream& os, S3Object const& obj)
    {
        os << obj.to_string();
        return os;
    }

    Map metadata;
    std::string m_container;
    std::string m_name;
    std::string m_user;
    std::string m_user_token;
    std::string m_region;
};
}  // namespace hestia