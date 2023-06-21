#pragma once

#include <ctime>
#include <string>

namespace hestia {

struct UserToken {
    std::string m_value;
    std::time_t m_creation_time{0};
};

class User {
  public:
    User() = default;

    User(const std::string& id) : m_identifier(id) {}

    const std::string& id() const { return m_identifier; }

    std::string m_identifier;
    std::string m_display_name;
    UserToken m_api_token;
    std::string m_password;
    std::time_t m_creation_time{0};
    std::time_t m_last_modified_time{0};
    bool m_is_admin{false};
};
}  // namespace hestia