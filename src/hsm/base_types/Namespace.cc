#include "Namespace.h"

namespace hestia {
Namespace::Namespace() : Model(HsmItem::namespace_name) {}

Namespace::Namespace(const std::string& name) : Model(HsmItem::namespace_name)
{
    m_name.update_value(name);
}

void Namespace::init()
{
    register_scalar_field(&m_name);
    register_scalar_field(&m_data);
}

std::string Namespace::get_type()
{
    return HsmItem::namespace_name;
}

}  // namespace hestia