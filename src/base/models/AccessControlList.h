#pragma once

#include "Serializeable.h"
#include "Uuid.h"

#include <map>

namespace hestia {
class AccessControlList : public Serializeable {
  public:
    struct UserPermission {
        bool m_read{false};
        bool m_write{false};
    };

    AccessControlList();

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    std::map<Uuid, UserPermission> m_user_permissions;
    std::map<Uuid, UserPermission> m_group_permissions;
};
}  // namespace hestia