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

std::pair<std::string, std::string> StringUtils::split_on_first(
    const std::string& str, const std::string& substr)
{
    if (str.empty()) {
        return {{}, {}};
    }

    if (substr.empty()) {
        return {str, {}};
    }

    const auto loc = str.find(substr);
    if (loc == str.npos) {
        return {str, {}};
    }
    return {
        str.substr(0, loc),
        str.substr(loc + substr.size(), str.size() - loc - substr.size())};
}


std::string StringUtils::remove_prefix(
    const std::string& str, const std::string& prefix)
{
    if (str == prefix) {
        return "";
    }

    if (const auto iter = str.find(prefix); iter != std::string::npos) {
        return str.substr(
            iter + prefix.length(), str.length() - prefix.length());
    }
    else {
        return str;
    }
}

bool StringUtils::starts_with(const std::string& str, const std::string& prefix)
{
    if (str == prefix) {
        return true;
    }
    if (prefix.empty()) {
        return false;
    }
    return str.find(prefix) == 0;
}

bool StringUtils::ends_with(const std::string& str, const std::string& prefix)
{
    const auto loc = str.rfind(prefix);
    if (loc == 0) {
        return false;
    }
    return loc == str.size() - prefix.size();
}

void StringUtils::split(
    const std::string& str, char delimiter, std::vector<std::string>& elements)
{
    std::string working_element;
    for (const auto c : str) {
        if (c == delimiter) {
            elements.push_back(working_element);
            working_element.clear();
        }
        else {
            working_element += c;
        }
    }
    if (!working_element.empty()) {
        elements.push_back(working_element);
    }
}

void StringUtils::split(
    const std::string& str,
    const std::string& delimiter,
    std::vector<std::string>& elements)
{
    if (str.empty() || delimiter.empty()) {
        return;
    }

    if (auto loc = str.find(delimiter); loc != std::string::npos) {
        elements.push_back(str.substr(0, loc));
        split(
            str.substr(
                loc + delimiter.size(), str.size() - loc - delimiter.size()),
            delimiter, elements);
    }
    else {
        elements.push_back(str);
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

std::string StringUtils::replace(
    const std::string& input, char to_replace, char replacement)
{
    std::string output;
    for (auto c : input) {
        if (c == to_replace) {
            output += replacement;
        }
        else {
            output += c;
        }
    }
    return output;
}
}  // namespace hestia