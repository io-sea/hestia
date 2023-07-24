#include "StorageObject.h"

#include "UuidUtils.h"

#include <chrono>

namespace hestia {

StorageObject::StorageObject(const std::string& key) :
    SerializeableWithFields(key, std::string(s_type))
{
    init();
}

void StorageObject::init()
{
    register_scalar_field(&m_size);
    register_scalar_field(&m_id);
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

void StorageObject::set_metadata(
    const std::string& key, const std::string& value)
{
    m_metadata.set_map_item(key, value);
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