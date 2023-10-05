#pragma once

#include <string>

namespace hestia {
struct S3Timestamp {
    S3Timestamp() = default;

    S3Timestamp(const std::string& value) : m_value(value) {}
    std::string m_value;
};

struct S3Range {
    S3Range() = default;
    S3Range(std::size_t offset, std::size_t length) :
        m_offset(offset), m_length(length)
    {
    }
    std::size_t m_offset{0};
    std::size_t m_length{0};
};
}  // namespace hestia