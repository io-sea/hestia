#pragma once

#include "Metadata.h"
#include <string>

namespace hestia {
class YamlUtils {
  public:
    static void load(const std::string& path, NestedMetadata& data);
};
}  // namespace hestia