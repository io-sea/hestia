#include "BaseField.h"

namespace hestia {

BaseField::BaseField(const std::string& name, IndexScope index_scope) :
    m_index_scope(index_scope), m_name(name)
{
}

const std::string& BaseField::get_name() const
{
    return m_name;
}

bool BaseField::modified() const
{
    return m_modified;
}

void BaseField::reset()
{
    m_modified = false;
}

bool BaseField::is_primary_key() const
{
    return m_is_primary_key;
}

BaseField::IndexScope BaseField::get_index_scope() const
{
    return m_index_scope;
}

void BaseField::set_is_primary_key(bool is_primary)
{
    m_is_primary_key = is_primary;
}

void BaseField::set_index_scope(IndexScope index_scope)
{
    m_index_scope = index_scope;
}

}  // namespace hestia