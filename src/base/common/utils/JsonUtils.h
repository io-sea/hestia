#pragma once

#include "Dictionary.h"

#include <filesystem>

namespace hestia {

/**
 * @brief Utilities for working with json
 */
class JsonUtils {
  public:
    /**
     * Return the values in the supplied json corresponding to the supplied
     * keys. Only top-level keys are checked.
     *
     * @param json a string with json content
     * @param keys the keys to get values for
     * @return The values in the order of supplied keys, if empty the key was not found
     */
    static std::vector<std::string> get_values(
        const std::string& json, const std::vector<std::string>& keys);

    /**
     * Convert the Metadata 'map' type to json
     *
     * @param metadata metadata to convert
     * @return The json as a string
     */
    static std::string to_json(const Map& metadata);

    static void to_json(
        const Dictionary& dict,
        std::string& output,
        const std::vector<std::string>& exclude_keys = {},
        unsigned indent                              = 0);

    static void from_json(
        const std::string& str,
        Dictionary& dict,
        const std::vector<std::string>& exclude_keys = {});

    static std::string to_json(
        const std::vector<Map>& metadata, const std::string& key = {});

    /**
     * Read Metadata from json - only reads the top level keys
     *
     * @param json json to read from
     * @param metadata to populate
     */
    static void from_json(const std::string& json, Map& metadata);

    static void from_json(const std::string& json, std::vector<Map>& metadata);

    static bool get_value(
        const std::filesystem::path& path,
        const std::string& key,
        std::string& value);

    static void get_values(
        const std::filesystem::path& path,
        const std::vector<std::string>& keys,
        std::vector<std::string>& values);

    static void set_value(
        const std::filesystem::path& path,
        const std::string& key,
        const std::string& value);

    static void set_values(
        const std::filesystem::path& path,
        const std::vector<KeyValuePair>& kv_pairs);

    static bool has_key(
        const std::filesystem::path& path, const std::string& key);

    static void has_keys(
        const std::filesystem::path& path,
        const std::vector<std::string>& keys,
        std::vector<bool>& found);

    static void remove_key(
        const std::filesystem::path& path, const std::string& key);

    static void remove_keys(
        const std::filesystem::path& path, const std::vector<std::string>& key);

    /**
     * Read the specified keys from a file containing json into Metadata
     *
     * @param path a path to the file to read
     * @param metadata to populate
     * @param keys only read these keys if supplied
     */
    static void read(
        const std::filesystem::path& path,
        Map& metadata,
        const std::vector<std::string>& keys = {});

    /**
     * Write the specified keys to a file in json format
     *
     * @param path a path to the file to write
     * @param metadata to with data
     * @param merge if true merge with the existing file content
     */
    static void write(
        const std::filesystem::path& path,
        const Map& metadata,
        bool merge = true);
};
}  // namespace hestia