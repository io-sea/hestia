#include "StringUtils.h"

#include <algorithm>
#include <sstream>

namespace hestia {
std::pair<std::string, std::string> StringUtils::split_on_first(
    const std::string& str, char delimiter)
{
    const auto loc = str.find_first_of(delimiter);
    if (loc == str.npos) {
        return {str, {}};
    }
    return {str.substr(0, loc), str.substr(loc + 1, str.size() - loc)};
}

std::string StringUtils::id_to_string(
    uint64_t lo, uint64_t hi, char delimiter, bool as_hex)
{
    std::stringstream sstr;
    if (as_hex) {
        sstr << std::hex << lo << delimiter << hi;
    }
    else {
        sstr << lo << delimiter << hi;
    }
    return sstr.str();
}

std::pair<uint64_t, uint64_t> StringUtils::string_to_id(
    const std::string& input, char delimiter, bool as_hex)
{
    std::stringstream ss;

    const auto loc = input.find(delimiter);
    if (loc == std::string::npos) {
        return {to_int(input), 0};
    }
    else {
        const auto lo_str = input.substr(0, loc);
        const auto hi_st  = input.substr(loc, input.size() - loc);
        return {to_int(lo_str, as_hex), to_int(hi_st, as_hex)};
    }
}

uint64_t StringUtils::to_int(const std::string& input, bool as_hex)
{
    if (as_hex) {
        uint64_t ret{0};
        std::stringstream ss;
        ss << std::hex << input;
        ss >> ret;
        return ret;
    }
    else {
        return std::stoi(input);
    }
}

std::string StringUtils::to_lower(const std::string& str)
{
    auto copy_str = str;
    std::transform(
        copy_str.begin(), copy_str.end(), copy_str.begin(), ::tolower);
    return copy_str;
}
}  // namespace hestia