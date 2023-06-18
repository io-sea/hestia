#include "Uuid.h"

#include "StringUtils.h"

namespace hestia {
Uuid::Uuid(uint64_t lo, uint64_t hi) : m_lo(lo), m_hi(hi) {}

Uuid::Uuid(const std::string& id, char delimiter, bool as_hex)
{
    from_string(id, delimiter, as_hex);
}

void Uuid::from_string(const std::string& str, char delimiter, bool as_hex)
{
    const auto [lo, hi] = StringUtils::string_to_id(str, delimiter, as_hex);
    m_lo                = lo;
    m_hi                = hi;
}

std::string Uuid::to_string(char delimiter, bool as_hex) const
{
    return StringUtils::id_to_string(m_lo, m_hi, delimiter, as_hex);
}
}  // namespace hestia