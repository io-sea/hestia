#pragma once

#include "HestiaNode.h"

#include <vector>

namespace hestia {
class HestiaNodeAdapter {
  public:
    static void to_json(const HestiaNode& node, std::string& json);

    static void from_json(const std::string& json, HestiaNode& node);

    static std::string to_json(const std::vector<HestiaNode>& nodes);
};
}  // namespace hestia