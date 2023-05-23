#pragma once

#include "Dictionary.h"
#include <string>

namespace YAML {  // NOLINT
class Node;
class Emitter;
}  // namespace YAML

namespace hestia {
class YamlUtils {
  public:
    /// @brief Load first YAML document in file at path
    ///
    /// @param path Path of YAML file to load
    /// @param dict Dictionary to read YAML to
    static void load(
        const std::string& path, Dictionary& dict);  // Load first doc in file

    /// @brief Load all YAML documents in file at path
    ///
    /// @param path Path of YAML file to load
    /// @param dicts Vector of dictionaries to read the YAML documents to
    static void load_all(
        const std::string& path,
        std::vector<std::unique_ptr<hestia::Dictionary>>& dicts);

    /// @brief Serialize a dictionary to a YAML string
    ///
    /// @param dict Dictionary to serialize
    /// @param yaml String to serialize to
    static void dict_to_yaml(
        const Dictionary& dict, std::string& yaml, const bool sorted = false);
};
}  // namespace hestia