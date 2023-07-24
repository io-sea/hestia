#include "BaseField.h"

namespace hestia {

BaseField::BaseField(const std::string& name, bool index_on) :
    m_index_on(index_on), m_name(name)
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

void BaseField::set_index_on(bool index_on)
{
    m_index_on = index_on;
}

void BaseField::set_is_primary_key(bool is_primary)
{
    m_is_primary_key = is_primary;
}

bool BaseField::should_index_on() const
{
    return m_index_on;
}

}  // namespace hestia