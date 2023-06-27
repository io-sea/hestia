#include "UserGroup.h"

namespace hestia {
UserGroup::UserGroup() : Model("user_group") {}

UserGroup::UserGroup(const std::string& name) : Model(name, "user_group") {}
}  // namespace hestia