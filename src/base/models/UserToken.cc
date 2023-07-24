#include "UserToken.h"

namespace hestia {
UserToken::UserToken(const std::string& created_by) : Model(s_model_type)
{
    m_user.set_id(created_by);
    init();
}

UserToken::UserToken(const std::string& id, const std::string& created_by) :
    Model(id, s_model_type)
{
    m_user.set_id(created_by);
    init();
}

UserToken::UserToken(const UserToken& other) : Model(other)
{
    *this = other;
}

UserToken& UserToken::operator=(const UserToken& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_value = other.m_value;
        m_user  = other.m_user;
        init();
    }
    return *this;
}

void UserToken::init()
{
    m_value.set_index_on(true);
    register_scalar_field(&m_value);
    register_named_foreign_key_field(&m_user);
}

std::string UserToken::get_type()
{
    return s_model_type;
}

const std::string& UserToken::get_user_id() const
{
    return m_user.get_id();
}

const std::string& UserToken::value() const
{
    return m_value.get_value();
}

void UserToken::set_value(const std::string& value)
{
    m_value.update_value(value);
}

}  // namespace hestia