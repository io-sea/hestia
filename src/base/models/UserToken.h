#pragma once

#include "Model.h"
#include "RelationshipField.h"

namespace hestia {
class UserToken : public Model {
  public:
    UserToken(const std::string& created_by = {});

    UserToken(const std::string& id, const std::string& created_by);

    UserToken(const UserToken& user);

    static std::string get_type();

    const std::string& get_user_id() const;

    void set_value(const std::string& value);

    const std::string& value() const;

    UserToken& operator=(const UserToken& other);

  private:
    void init();

    static constexpr const char s_model_type[]{"user_token"};
    StringField m_value{"value"};
    ForeignKeyField m_user{"user", "user"};
};

}  // namespace hestia