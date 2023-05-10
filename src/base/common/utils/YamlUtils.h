#pragma once

#include "Dictionary.h"
#include <string>

namespace YAML {  // NOLINT
class Node;
}

namespace hestia {
class YamlUtils {
  public:
    static void load(const std::string& path, Dictionary& dict);

  private:
    static void on_map(const YAML::Node& node, Dictionary& dict);

    static void on_sequence(const YAML::Node& node, Dictionary& dict);
};
}  // namespace hestia