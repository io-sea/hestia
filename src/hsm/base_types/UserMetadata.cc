#include "UserMetadata.h"

namespace hestia {
UserMetadata::UserMetadata() :
    HsmItem(HsmItem::Type::METADATA), Model(HsmItem::user_metadata_name)
{
}

std::string UserMetadata::get_type()
{
    return HsmItem::user_metadata_name;
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