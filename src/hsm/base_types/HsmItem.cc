#include "HsmItem.h"

namespace hestia {

const std::vector<HsmItem::Type> HsmItem::s_all_items = {
    Type::DATASET,   Type::OBJECT, Type::ACTION, Type::EVENT,
    Type::NAMESPACE, Type::TIER,   Type::EXTENT, Type::METADATA};

HsmItem::HsmItem(HsmItem::Type type) : m_hsm_type(type) {}

HsmItem::Type HsmItem::get_hsm_type() const
{
    return m_hsm_type;
}

std::string HsmItem::to_name(Type type)
{
    switch (type) {
        case Type::DATASET:
            return dataset_name;
        case Type::OBJECT:
            return hsm_object_name;
        case Type::ACTION:
            return hsm_action_name;
        case Type::EVENT:
            return hsm_event_name;
        case Type::NAMESPACE:
            return namespace_name;
        case Type::TIER:
            return tier_name;
        case Type::EXTENT:
            return tier_extents_name;
        case Type::METADATA:
            return user_metadata_name;
        case Type::UNKNOWN:
            return "";
        default:
            return "";
    }
}

HsmItem::Type HsmItem::from_name(const std::string& type_name)
{
    if (type_name == dataset_name) {
        return Type::DATASET;
    }
    else if (type_name == hsm_object_name) {
        return Type::OBJECT;
    }
    else if (type_name == dataset_name) {
        return Type::ACTION;
    }
    else if (type_name == hsm_action_name) {
        return Type::EVENT;
    }
    else if (type_name == hsm_event_name) {
        return Type::DATASET;
    }
    else if (type_name == namespace_name) {
        return Type::NAMESPACE;
    }
    else if (type_name == tier_name) {
        return Type::TIER;
    }
    else if (type_name == tier_extents_name) {
        return Type::EXTENT;
    }
    else if (type_name == user_metadata_name) {
        return Type::METADATA;
    }
    else {
        return Type::UNKNOWN;
    }
}

std::vector<HsmItem::Type> HsmItem::get_all_items()
{
    return s_all_items;
}
}  // namespace hestia
