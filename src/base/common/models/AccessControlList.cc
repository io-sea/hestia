#include "AccessControlList.h"

#include "UuidUtils.h"

namespace hestia {
void AccessControlList::deserialize(const Dictionary& dict)
{
    auto user_permission = dict.get_map_item("user");
    if (user_permission != nullptr
        && user_permission->get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& perm_entry : user_permission->get_sequence()) {
            Uuid id;
            UserPermission permission;
            auto on_item = [&id, &permission](
                               const std::string& key,
                               const std::string& value) {
                if (key == "id" && !value.empty()) {
                    id = UuidUtils::from_string(value);
                }
                else if (key == "read" && !value.empty()) {
                    permission.m_read = value == "true";
                }
                else if (key == "write" && !value.empty()) {
                    permission.m_write = value == "true";
                }
            };
            perm_entry->for_each_scalar(on_item);
            if (!id.is_unset()) {
                m_user_permissions[id] = permission;
            }
        }
    }

    auto group_permission = dict.get_map_item("group");
    if (group_permission != nullptr
        && group_permission->get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& perm_entry : group_permission->get_sequence()) {
            Uuid id;
            UserPermission permission;
            auto on_item = [&id, &permission](
                               const std::string& key,
                               const std::string& value) {
                if (key == "id" && !value.empty()) {
                    id = UuidUtils::from_string(value);
                }
                else if (key == "read" && !value.empty()) {
                    permission.m_read = value == "true";
                }
                else if (key == "write" && !value.empty()) {
                    permission.m_write = value == "true";
                }
            };
            perm_entry->for_each_scalar(on_item);
            if (!id.is_unset()) {
                m_group_permissions[id] = permission;
            }
        }
    }
}

void AccessControlList::serialize(Dictionary& dict) const
{
    auto user_seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& [uuid, permission] : m_user_permissions) {
        auto perm_entry = std::make_unique<Dictionary>();
        std::unordered_map<std::string, std::string> data;
        data["id"]    = UuidUtils::to_string(uuid);
        data["read"]  = permission.m_read ? "true" : "false";
        data["write"] = permission.m_write ? "true" : "false";
        perm_entry->set_map(data);
        user_seq->add_sequence_item(std::move(perm_entry));
    }
    dict.set_map_item("user", std::move(user_seq));

    auto group_seq = std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
    for (const auto& [uuid, permission] : m_group_permissions) {
        auto perm_entry = std::make_unique<Dictionary>();
        std::unordered_map<std::string, std::string> data;
        data["id"]    = UuidUtils::to_string(uuid);
        data["read"]  = permission.m_read ? "true" : "false";
        data["write"] = permission.m_write ? "true" : "false";
        perm_entry->set_map(data);
        group_seq->add_sequence_item(std::move(perm_entry));
    }
    dict.set_map_item("group", std::move(group_seq));
}
}  // namespace hestia