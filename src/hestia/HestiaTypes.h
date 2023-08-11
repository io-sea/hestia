#pragma once

#include "EnumUtils.h"
#include "HsmItem.h"

namespace hestia {
class HestiaType {
  public:
    enum class Type { SYSTEM, HSM };

    STRINGABLE_ENUM(SystemType, USER, HSM_NODE)

    HestiaType() = default;

    HestiaType(HsmItem::Type hsm_type);

    HestiaType(SystemType system_type);

    static std::string to_string(HestiaType type);

    Type m_type{Type::HSM};
    HsmItem::Type m_hsm_type{HsmItem::Type::OBJECT};
    SystemType m_system_type{SystemType::USER};
};

}  // namespace hestia