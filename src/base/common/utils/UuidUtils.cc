#include "UuidUtils.h"

#include "StringUtils.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <iostream>

namespace hestia {
Uuid UuidUtils::from_string(
    const std::string& input, Uuid::Format format, char delimiter)
{
    if (format == Uuid::Format::Decimal64 || format == Uuid::Format::Hex64) {
        const bool as_hex = format == Uuid::Format::Hex64;

        std::stringstream ss;
        const auto loc = input.find(delimiter);
        if (loc == std::string::npos) {
            return {StringUtils::to_int(input, as_hex), 0};
        }
        else {
            const auto lo_str = input.substr(0, loc);
            const auto hi_st  = input.substr(loc, input.size() - loc);
            return {
                StringUtils::to_int(lo_str, as_hex),
                StringUtils::to_int(hi_st, as_hex)};
        }
    }
    else if (format == Uuid::Format::Bytes16) {
        if (input.length() != 16) {
            throw std::runtime_error(
                "Can't convert string format Bytes16 of length: "
                + std::to_string(input.size()) + " to uuid");
        }

        uint64_t lo = 0;
        for (std::size_t idx = 8; idx < 16; idx++) {
            uint64_t c = input[idx];
            lo |= (c << ((idx - 8) * 8));
        }

        uint64_t hi = 0;
        for (std::size_t idx = 0; idx < 8; idx++) {
            uint64_t c = input[idx];
            hi |= (c << (idx * 8));
        }
        return {lo, hi};
    }
    else if (format == Uuid::Format::Hex8_4_4_4_12) {
        if (input.length() != 36) {
            throw std::runtime_error(
                "Can't convert string format Hex8_4_4_4_12 of length: "
                + std::to_string(input.size())
                + " to uuid. Input is: " + input);
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
    return {};
}

std::string UuidUtils::to_string(
    const Uuid& uuid, Uuid::Format format, char delimiter)
{
    if (format == Uuid::Format::Decimal64 || format == Uuid::Format::Hex64) {
        const bool as_hex = format == Uuid::Format::Hex64;

        std::stringstream sstr;
        if (as_hex) {
            sstr << std::hex << uuid.m_lo << delimiter << uuid.m_hi;
        }
        else {
            sstr << uuid.m_lo << delimiter << uuid.m_hi;
        }
        return sstr.str();
    }
    else if (format == Uuid::Format::Hex8_4_4_4_12) {
        std::vector<unsigned char> bytes(16);
        for (std::size_t idx = 0; idx < 8; idx++) {
            bytes[idx] = uuid.m_lo >> (idx * 8);
        }
        for (std::size_t idx = 0; idx < 8; idx++) {
            bytes[8 + idx] = uuid.m_hi >> (idx * 8);
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
    return {};
}
}  // namespace hestia
