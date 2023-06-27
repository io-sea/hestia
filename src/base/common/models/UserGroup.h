#pragma once

#include "Model.h"
#include "User.h"

namespace hestia {
class UserGroup : public Model {
  public:
    UserGroup();

    UserGroup(const std::string& name);

    std::string m_name;
    std::vector<User> m_users;
};
}  // namespace hestia