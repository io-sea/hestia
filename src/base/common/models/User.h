#pragma once

#include "Model.h"

namespace hestia {

struct UserToken {
    std::string m_value;
    std::time_t m_creation_time{0};
};

class User : public Model {
  public:
    User();

    User(const Uuid& id);

    User(const std::string& name);

    void deserialize(
        const Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL) override;

    void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const override;

    std::string m_display_name;
    UserToken m_api_token;
    std::string m_password;
    bool m_is_admin{false};
};
}  // namespace hestia