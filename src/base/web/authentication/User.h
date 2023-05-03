#pragma once

#include <string>

namespace hestia {
class User {
  public:
    std::string m_identifier;
    std::string m_token;
};
}  // namespace hestia