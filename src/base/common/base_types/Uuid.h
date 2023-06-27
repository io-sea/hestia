#pragma once

#include <string>

namespace hestia {

/**
 * @brief A 'UUID' convenience class
 *
 * Class to help handle UUIDs - has a focus on a two-integer representation
 * using in data storage.
 */
class Uuid {
  public:
    enum class Format { Bytes16, Decimal64, Hex64, Hex8_4_4_4_12 };

    Uuid();

    /**
     * Constructor
     *
     * @param lo The lower 64 bits of the 128 bit UUID
     * @param hi The higher 64 bits of the 128 bit UUID
     */
    Uuid(uint64_t lo, uint64_t hi = 0);

    bool is_unset() const;

    std::string to_string() const;

    bool operator==(const Uuid& other) const
    {
        return other.m_lo == m_lo && other.m_hi == m_hi;
    }

    bool operator<(const Uuid& other) const
    {
        if (m_hi < other.m_hi) {
            return true;
        }
        else if (m_hi > other.m_hi) {
            return false;
        }
        if (m_lo < other.m_lo) {
            return true;
        }
        else {
            return false;
        }
    }

    uint64_t m_lo{0};
    uint64_t m_hi{0};

  private:
    bool m_is_unset{true};
};
}  // namespace hestia
