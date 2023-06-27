#include "S3Object.h"

#include <ctime>

namespace hestia {
S3Object::S3Object(const std::string& name) : m_name(name) {}

std::string S3Object::to_string() const
{
    std::string ret;
    ret += "Name: " + m_name + "\n";
    return ret;
}

bool S3Object::is_equal(const S3Object& other) const
{
    return m_name == other.m_name;
}
}  // namespace hestia