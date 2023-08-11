#pragma once

#include "AccessControlList.h"
#include "Model.h"
#include "RelationshipField.h"
#include "User.h"

namespace hestia {
class OwnableModel : public Model {
  public:
    OwnableModel(
        const std::string& id,
        const std::string& type,
        const std::string& created_by);

    OwnableModel(const std::string& type, const std::string& created_by);

    OwnableModel(const OwnableModel& other);

    const std::string& get_created_by() const;

    OwnableModel& operator=(const OwnableModel& other);

  protected:
    ForeignKeyField m_created_by{"created_by", User::get_type()};
    TypedDictField<AccessControlList> m_permissions{"acl"};

  private:
    void init();
};
}  // namespace hestia