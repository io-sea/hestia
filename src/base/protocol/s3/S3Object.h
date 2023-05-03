#pragma once

#include "Metadata.h"

namespace hestia {
class S3Object {
  public:
    S3Object(const std::string& key = {});

    bool empty() const;

    bool is_equal(const S3Object& other);

    std::string to_string() const;

    bool operator==(const S3Object& other) { return is_equal(other); }

    friend std::ostream& operator<<(std::ostream& os, S3Object const& value)
    {
        os << value.to_string();
        return os;
    }

    std::string m_key;
    std::string m_creation_time;
    std::string m_content_length = "0";
    std::string m_container;

    Metadata m_user_metadata;
};
}  // namespace hestia