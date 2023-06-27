#include "Uuid.h"

#include "StringUtils.h"

#include <sstream>

namespace hestia {
Uuid::Uuid() {}

Uuid::Uuid(uint64_t lo, uint64_t hi) : m_lo(lo), m_hi(hi), m_is_unset(false) {}

bool Uuid::is_unset() const
{
    return m_is_unset;
}

std::string Uuid::to_string() const
{
    std::stringstream sstr;
    sstr << std::hex << m_lo << '-' << m_hi;
    return sstr.str();
}
}  // namespace hestia