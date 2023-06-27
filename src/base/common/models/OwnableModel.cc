#include "OwnableModel.h"

namespace hestia {

OwnableModel::OwnableModel(const Uuid& id) : Model(id) {}

OwnableModel::OwnableModel(const std::string& type) : Model(type) {}

OwnableModel::OwnableModel(const std::string& key, const std::string& type) :
    Model(key, type)
{
}

void OwnableModel::deserialize(const Dictionary& dict, SerializeFormat format)
{
    Model::deserialize(dict, format);
    if (format == SerializeFormat::ID) {
        return;
    }

    auto user_dict = dict.get_map_item("created_by");
    if (user_dict != nullptr) {
        if (format == SerializeFormat::CHILD_ID) {
            m_created_by.deserialize(*user_dict, SerializeFormat::ID);
        }
        else {
            m_created_by.deserialize(*user_dict, SerializeFormat::FULL);
        }
    }

    auto acl = dict.get_map_item("acl");
    if (acl != nullptr) {
        m_permissions.deserialize(*acl);
    }
}

void OwnableModel::serialize(
    Dictionary& dict, SerializeFormat format, const Uuid& id) const
{
    Model::serialize(dict, format, id);

    if (format == SerializeFormat::ID) {
        return;
    }

    auto user_dict = std::make_unique<Dictionary>();
    if (format == SerializeFormat::CHILD_ID) {
        m_created_by.serialize(*user_dict, SerializeFormat::ID);
    }
    else {
        m_created_by.serialize(*user_dict, SerializeFormat::FULL);
    }
    dict.set_map_item("created_by", std::move(user_dict));

    auto acl_dict = std::make_unique<Dictionary>();
    m_permissions.serialize(*acl_dict);
    dict.set_map_item("acl", std::move(acl_dict));
}
}  // namespace hestia