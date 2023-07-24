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
        return Uuid::from_string(input, delimiter);
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
            sstr << std::hex << uuid.lo() << delimiter << uuid.hi();
        }
        else {
            sstr << uuid.lo() << delimiter << uuid.hi();
        }
        return sstr.str();
    }
    else if (format == Uuid::Format::Hex8_4_4_4_12) {
        return uuid.to_string(delimiter);
    }
    return {};
}
}  // namespace hestia
