#include "StorageObject.h"

#include <chrono>

namespace hestia {

StorageObject::StorageObject(const std::string& id) : m_id(id)
{
    initialize_timestamps();
}

bool StorageObject::empty() const
{
    return m_size == 0;
}

const std::string& StorageObject::id() const
{
    return m_id;
}

bool StorageObject::valid() const
{
    return !m_id.empty();
}

void StorageObject::update_modified_time(std::time_t time)
{
    m_last_modified_time = time;
}

void StorageObject::update_modified_time()
{
    m_last_modified_time =
        std::chrono::system_clock::now().time_since_epoch().count();
}

void StorageObject::initialize_timestamps()
{
    m_creation_time =
        std::chrono::system_clock::now().time_since_epoch().count();
    update_modified_time(m_creation_time);
}

std::string StorageObject::to_string() const
{
    std::string ret;
    ret += "Id: " + m_id + " | ";
    ret += "Metadata: {";
    ret += m_metadata.to_string() + "}";
    return ret;
}

}  // namespace hestia