#pragma once

#include "Dictionary.h"
#include "HsmNode.h"
#include "StringAdapter.h"

#include <vector>

namespace hestia {
class HsmNodeJsonAdapter : public StringAdapter<HsmNode> {
  public:
    void to_string(
        const HsmNode& item,
        std::string& json,
        const std::string& id = {}) const override;

    void to_string(
        const std::vector<HsmNode>& items, std::string& output) const override;

    void from_string(const std::string& json, HsmNode& item) const override;

    void from_string(
        const std::string& json, std::vector<HsmNode>& item) const override;

    bool matches_query(
        const HsmNode& item, const Metadata& query) const override;

  private:
    static void to_dict(
        const HsmNode& item, Dictionary& dict, const std::string& id = {});

    static void from_dict(const Dictionary& dict, HsmNode& item);
};
}  // namespace hestia