#include "HestiaTypes.h"

#include "User.h"

namespace hestia {
HestiaType::HestiaType(HsmItem::Type hsm_type) :
    m_type{Type::HSM}, m_hsm_type(hsm_type)
{
}

HestiaType::HestiaType(SystemType system_type) :
    m_type{Type::SYSTEM}, m_system_type(system_type)
{
}

std::string HestiaType::to_string(HestiaType type)
{
    if (type.m_type == HestiaType::Type::SYSTEM) {
        return User::get_type();
    }
    else {
        return HsmItem::to_name(type.m_hsm_type);
    }
}
}  // namespace hestia