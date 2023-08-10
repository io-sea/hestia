#pragma once

#include "UserToken.h"

namespace hestia {

class User : public Model {
  public:
    User();

    User(const std::string& id);

    User(const User& user);

    bool is_admin() const;

    static std::string get_type();

    const std::string& password() const;

    const std::vector<UserToken>& tokens() const;

    const UserToken& get_first_token() const;

    void add_token(const UserToken& token);

    void set_password(const std::string& password);

    User& operator=(const User& other);

  private:
    void init();

    static constexpr const char s_model_type[]{"user"};
    StringField m_display_name{"display_name"};
    StringField m_password{"password"};
    BooleanField m_is_admin{"is_admin"};
    ForeignKeyProxyField<UserToken> m_tokens{"tokens"};
};

}  // namespace hestia