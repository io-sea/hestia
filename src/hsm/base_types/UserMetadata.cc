#include "UserMetadata.h"

namespace hestia {
UserMetadata::UserMetadata() :
    HsmItem(HsmItem::Type::METADATA), Model(HsmItem::user_metadata_name)
{
    init();
}

UserMetadata::UserMetadata(const UserMetadata& other) :
    HsmItem(HsmItem::Type::METADATA), Model(other)
{
    *this = other;
}

UserMetadata& UserMetadata::operator=(const UserMetadata& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_data   = other.m_data;
        m_object = other.m_object;
        init();
    }
    return *this;
}

void UserMetadata::init()
{
    register_map_field(&m_data);
    register_foreign_key_field(&m_object);
}

std::string UserMetadata::get_type()
{
    return HsmItem::user_metadata_name;
}

void UserMetadata::set_item(const std::string& key, const std::string& value)
{
    m_data.get_map_as_writeable().set_item(key, value);
}

const Map& UserMetadata::data() const
{
    return m_data.get_map();
}

const std::string& UserMetadata::object() const
{
    return m_object.get_id();
}
}  // namespace hestia