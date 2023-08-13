#pragma once

#include <string>

namespace hestia {
class AuthorizationContext {
  public:
    std::string m_user_id;
    std::string m_user_token;
};
}  // namespace hestia