#include "StorageObject.h"

#include "UuidUtils.h"

#include <chrono>

namespace hestia {

StorageObject::StorageObject(const std::string& key) :
    SerializeableWithFields(key, std::string(s_type))
{
    init();
}

StorageObject::StorageObject(const StorageObject& other) :
    SerializeableWithFields(other)
{
    *this = other;
}

StorageObject& StorageObject::operator=(const StorageObject& other)
{
    if (this != &other) {
        SerializeableWithFields::operator=(other);
        m_location = other.m_location;
        m_metadata = other.m_metadata;
        m_size     = other.m_size;
        init();
    }
    return *this;
}

void StorageObject::init()
{
    register_scalar_field(&m_size);
    register_scalar_field(&m_location);
    register_map_field(&m_metadata);
}

const Map& StorageObject::metadata() const
{
    return m_metadata.get_map();
}

Map& StorageObject::get_metadata_as_writeable()
{
    return m_metadata.get_map_as_writeable();
}

const std::string& StorageObject::get_location() const
{
    return m_location.get_value();
}

std::string StorageObject::get_metadata_item(const std::string& key) const
{
    return m_metadata.get_map().get_item(key);
}

void StorageObject::set_metadata(
    const std::string& key, const std::string& value)
{
    m_metadata.set_map_item(key, value);
}

void StorageObject::set_location(const std::string& address)
{
    m_location.update_value(address);
}

bool StorageObject::empty() const
{
    return m_size.get_value() == 0;
}

std::string StorageObject::to_string() const
{
    std::string ret;
    ret += "Id: " + m_id.get_value() + " | ";
    ret += "Metadata: {";
    ret += m_metadata.get_map().to_string() + "}";
    return ret;
}

}  // namespace hestia