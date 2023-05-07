#include "S3Object.h"

#include <ctime>

namespace hestia {
S3Object::S3Object(const std::string& key) : m_key(key)
{
    m_creation_time = std::to_string(std::time(nullptr));
}

bool S3Object::empty() const
{
    return m_content_length == "0";
}

std::string S3Object::to_string() const
{
    std::string ret;
    ret += "Key: " + m_key + "\n";
    ret += "Creation Time: " + m_creation_time + "\n";
    ret += "Content Length: " + m_content_length + "\n";
    ret += "Container: " + m_container + "\n";
    ret += "Metadata: " + m_user_metadata.to_string();
    return ret;
}

bool S3Object::is_equal(const S3Object& other) const
{
    return m_key == other.m_key && m_creation_time == other.m_creation_time
           && m_content_length == other.m_content_length
           && m_container == other.m_container
           && m_user_metadata == other.m_user_metadata;
}
}  // namespace hestia