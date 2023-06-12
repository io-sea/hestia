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
     * Convert a UUID speficied by lo and hi 64 bit elements to a string. Use
     * the UUID class instead of this.
     *
     * @param lo the lower 64 bits
     * @param hi the higher 64 bits
     * @param delimiter character betweeen lo and hi bits
     * @param as_hex if true write as hex
     * @return The uuid as a string
     */
    static std::string id_to_string(
        uint64_t lo, uint64_t hi, char delimiter, bool as_hex = true);

    /**
     * Convert a string to a UUID. Use the UUID class instead of this.
     *
     * @param input the string to convert
     * @param delimiter character betweeen lo and hi bits
     * @param as_hex if true write as hex
     * @return the UUID as a pair of 64 bit ints, lo bits are first in the pair.
     */
    static std::pair<uint64_t, uint64_t> string_to_id(
        const std::string& input, char delimiter, bool as_hex = true);

    /**
     * Split the string on the first occurence of the delimiter
     *
     * @param str the string to split
     * @param delimiter delimiter
     * @return the elements of the string before and after the delimiter, either can be empty.
     */
    static std::pair<std::string, std::string> split_on_first(
        const std::string& str, char delimiter);

    static void split(
        const std::string& str,
        char delimiter,
        std::vector<std::string>& elements);

    /**
     * Convert the string to lower case
     *
     * @param str the string to convert to lower
     * @return the string in lower case
     */
    static std::string to_lower(const std::string& str);

    static bool starts_with(const std::string& str, const std::string& prefix);

    static std::string remove_prefix(
        const std::string& str, const std::string& prefix);

    static std::string replace(
        const std::string& input, char to_replace, char replacement);
};
}  // namespace hestia