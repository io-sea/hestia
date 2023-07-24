#include "Uuid.h"

#include "StringUtils.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace hestia {
Uuid::Uuid() {}

Uuid::Uuid(uint64_t lo, uint64_t hi) : m_lo(lo), m_hi(hi), m_is_unset(false) {}

bool Uuid::is_unset() const
{
    return m_is_unset;
}

std::string Uuid::to_string(char delimiter) const
{
    std::vector<unsigned char> bytes(16);
    for (std::size_t idx = 0; idx < 8; idx++) {
        bytes[idx] = m_lo >> (idx * 8);
    }
    for (std::size_t idx = 0; idx < 8; idx++) {
        bytes[8 + idx] = m_hi >> (idx * 8);
    }

    std::stringstream sstr;
    for (std::size_t idx = 0; idx < 4; idx++) {
        sstr << std::setfill('0') << std::setw(2) << std::hex
             << static_cast<int>(bytes[15 - idx]);
    }
    sstr << delimiter;
    for (std::size_t idx = 0; idx < 2; idx++) {
        sstr << std::setfill('0') << std::setw(2) << std::hex
             << static_cast<int>(bytes[11 - idx]);
    }
    sstr << delimiter;
    for (std::size_t idx = 0; idx < 2; idx++) {
        sstr << std::setfill('0') << std::setw(2) << std::hex
             << static_cast<int>(bytes[9 - idx]);
    }
    sstr << delimiter;
    for (std::size_t idx = 0; idx < 2; idx++) {
        sstr << std::setfill('0') << std::setw(2) << std::hex
             << static_cast<int>(bytes[7 - idx]);
    }
    sstr << delimiter;
    for (std::size_t idx = 0; idx < 6; idx++) {
        sstr << std::setfill('0') << std::setw(2) << std::hex
             << static_cast<int>(bytes[5 - idx]);
    }
    return sstr.str();
}

Uuid Uuid::from_string(const std::string& input, char delimiter)
{
    if (input.length() != 36) {
        throw std::runtime_error(
            "Can't convert string format Hex8_4_4_4_12 of length: "
            + std::to_string(input.size()) + " to uuid. Input is: " + input);
    }

    if ((input[8] != delimiter) || (input[13] != delimiter)
        || (input[18] != delimiter) || (input[23] != delimiter)) {
        throw std::runtime_error(
            "Unexpected content converting string format Hex8_4_4_4_12 to uuid");
    }

    uint64_t buffer{0};
    std::stringstream sstr;
    sstr << input.substr(24, 12);
    sstr >> std::hex >> buffer;
    uint64_t lo = buffer;

    sstr.clear();
    sstr << input.substr(19, 4);
    sstr >> std::hex >> buffer;
    lo |= (buffer << 48);

    sstr.clear();
    sstr << input.substr(14, 4);
    sstr >> std::hex >> buffer;
    uint64_t hi = buffer;

    sstr.clear();
    sstr << input.substr(9, 4);
    sstr >> std::hex >> buffer;
    hi |= (buffer << 16);

    sstr.clear();
    sstr << input.substr(0, 8);
    sstr >> std::hex >> buffer;
    hi |= (buffer << 32);
    return {lo, hi};
}

}  // namespace hestia