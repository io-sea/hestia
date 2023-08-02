#pragma once

#include "Model.h"
#include "RelationshipField.h"

#include "HsmItem.h"

namespace hestia {
class UserMetadata : public HsmItem, public Model {
  public:
    UserMetadata();

    static std::string get_type();

    const Map& data() const;

    const std::string& object() const;

  private:
    ScalarMapField m_data{"data"};

    ForeignKeyField m_object{"object", HsmItem::hsm_object_name};
};
}  // namespace hestia