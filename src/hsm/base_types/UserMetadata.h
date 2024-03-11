#pragma once

#include "Model.h"
#include "RelationshipField.h"

#include "HsmItem.h"

namespace hestia {
class UserMetadata : public HsmItem, public Model {
  public:
    UserMetadata();

    UserMetadata(const UserMetadata& other);

    static std::string get_type();

    const Map& data() const;

    const std::string& object() const;

    void set_item(const std::string& key, const std::string& value);

    UserMetadata& operator=(const UserMetadata& other);

  private:
    void init();

    ScalarMapField m_data{"data"};

    ForeignKeyField m_object{"object", HsmItem::hsm_object_name};
};
}  // namespace hestia