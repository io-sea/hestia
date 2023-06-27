#pragma once

#include <string>
#include <vector>

namespace hestia {

/**
 * @brief Utilities for working with strings
 */
class StringUtils {
  public:
    /**
     * Convert the string to int type, can handle hex if specified.
     * Intent is to eventually handle general string - int translation,
     * validation and errors.
     *
     * @param input the string to convert
     * @param as_hex if true the string content is taken as hex
     * @return The string as int
     */
    static uint64_t to_int(const std::string& input, bool as_hex = false);

    /**
     * Split the string on the first occurence of the delimiter
     *
     * @param str the string to split
     * @param delimiter delimiter
     * @return the elements of the string before and after the delimiter, either can be empty.
     */
    static std::pair<std::string, std::string> split_on_first(
        const std::string& str, char delimiter);

    /**
     * Split the string on the first occurence of the substring
     *
     * @param str the string to split
     * @param substr substring
     * @return the elements of the string before and after the substring, either can be empty.
     */
    static std::pair<std::string, std::string> split_on_first(
        const std::string& str, const std::string& substr);

    static void split(
        const std::string& str,
        char delimiter,
        std::vector<std::string>& elements);

    static void split(
        const std::string& str,
        const std::string& delimiter,
        std::vector<std::string>& elements);

    /**
     * Convert the string to lower case
     *
     * @param str the string to convert to lower
     * @return the string in lower case
     */
    static std::string to_lower(const std::string& str);

    static bool starts_with(const std::string& str, const std::string& prefix);

    static bool ends_with(const std::string& str, const std::string& prefix);

    static std::string remove_prefix(
        const std::string& str, const std::string& prefix);

    static std::string replace(
        const std::string& input, char to_replace, char replacement);
};
}  // namespace hestia