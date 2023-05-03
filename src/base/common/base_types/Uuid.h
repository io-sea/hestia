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
    /**
     * Constructor
     *
     * @param lo The lower 64 bits of the 128 bit UUID
     * @param hi The higher 64 bits of the 128 bit UUID
     */
    Uuid(uint64_t lo = 0, uint64_t hi = 0);

    /**
     * Update the UUID based on the provided string - format is
     * {HIGH}{DELIMITER}{LOW} with {HIGH} and {LOW} representing the higher and
     * lower 64 bits and a delimiting character in between. 123_456 is a valid
     * example, as is 01234567_76543210.
     *
     * @param str A string representation of the UUID
     * @param delimiter The higher 64 bits of the 128 bit UUID
     * @param as_hex if true expect the HIGH and LOW elements to be in hex format
     */
    void from_string(
        const std::string& str, char delimiter = '_', bool as_hex = true);

    /**
     * Return a string representation of the UUID in the form
     * {HIGH}{DELIMITER}{LOW}
     *
     * @param delimiter The higher 64 bits of the 128 bit UUID
     * @param as_hex if true expect the HIGH and LOW elements to be in hex format
     * @return a string representation of the UUID
     */
    std::string to_string(char delimiter = '_', bool as_hex = true) const;

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
};
}  // namespace hestia
