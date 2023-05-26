#pragma once

#include "HsmNode.h"
#include "StringAdapter.h"

#include <vector>

namespace hestia {
class HsmNodeJsonAdapter : public StringAdapter<HsmNode> {
  public:
    void to_string(const HsmNode& node, std::string& json) const;

    void from_string(const std::string& json, HsmNode& node) const;

    static std::string to_json(const std::vector<HsmNode>& nodes);
};
}  // namespace hestia