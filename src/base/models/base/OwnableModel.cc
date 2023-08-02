#include "OwnableModel.h"

namespace hestia {

OwnableModel::OwnableModel(
    const std::string& type, const std::string& created_by) :
    Model(type)
{
    set_has_owner(true);

    if (!created_by.empty()) {
        m_created_by.set_id(created_by);
    }
    init();
}

OwnableModel::OwnableModel(
    const std::string& id,
    const std::string& type,
    const std::string& created_by) :
    Model(id, type)
{
    set_has_owner(true);

    if (!created_by.empty()) {
        m_created_by.set_id(created_by);
    }
    init();
}

OwnableModel::OwnableModel(const OwnableModel& other) : Model(other)
{
    *this = other;
}

OwnableModel& OwnableModel::operator=(const OwnableModel& other)
{
    if (this != &other) {
        Model::operator=(other);
        m_created_by  = other.m_created_by;
        m_permissions = other.m_permissions;
        init();
    }
    return *this;
}

void OwnableModel::init()
{
    register_foreign_key_field(&m_created_by);
    register_map_field(&m_permissions);
}

const std::string& OwnableModel::get_created_by() const
{
    return m_created_by.get_id();
}


}  // namespace hestia