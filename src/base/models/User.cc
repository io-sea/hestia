#include "User.h"

namespace hestia {

User::User() : Model(s_model_type)
{
    init();
}

User::User(const std::string& id) : Model(id, s_model_type)
{
    init();
}

User::User(const User& other) : Model(other)
{
    *this = other;
}

std::string User::get_type()
{
    return s_model_type;
}

User& User::operator=(const User& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_display_name = other.m_display_name;
        m_password     = other.m_password;
        m_is_admin     = other.m_is_admin;
        m_tokens       = other.m_tokens;
        init();
    }
    return *this;
}

void User::init()
{
    m_name.set_index_on(true);

    register_scalar_field(&m_display_name);
    register_scalar_field(&m_password);
    register_scalar_field(&m_is_admin);

    register_foreign_key_proxy_field(&m_tokens);
}

bool User::is_admin() const
{
    return m_is_admin.get_value();
}

const std::string& User::password() const
{
    return m_password.get_value();
}

const std::vector<UserToken>& User::tokens() const
{
    return m_tokens.models();
}

void User::set_password(const std::string& password)
{
    m_password.update_value(password);
}

}  // namespace hestia