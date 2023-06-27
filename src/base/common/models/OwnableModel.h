#pragma once

#include "Model.h"
#include "User.h"

#include "AccessControlList.h"

namespace hestia {
class OwnableModel : public Model {
  public:
    OwnableModel(const Uuid& id);

    OwnableModel(const std::string& type);

    OwnableModel(const std::string& key, const std::string& type);

    void deserialize(
        const Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL) override;

    void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const override;

    User m_created_by;
    AccessControlList m_permissions;
};
}  // namespace hestia