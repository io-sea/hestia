#pragma once

#include "Dictionary.h"
#include "Uuid.h"

#include <map>

namespace hestia {
class AccessControlList {
  public:
    struct UserPermission {
        bool m_read{false};
        bool m_write{false};
    };

    void deserialize(const Dictionary& dict);

    void serialize(Dictionary& dict) const;

    std::map<Uuid, UserPermission> m_user_permissions;
    std::map<Uuid, UserPermission> m_group_permissions;
};
}  // namespace hestia