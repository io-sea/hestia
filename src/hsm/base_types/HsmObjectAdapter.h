#pragma once

#include "Dictionary.h"
#include "HsmObject.h"
#include "StringAdapter.h"

#include <memory>
#include <sstream>

namespace hestia {

class HsmObjectJsonAdapter : public StringAdapter<HsmObject> {
  public:
    HsmObjectJsonAdapter(const std::string& internal_key_prefix);

    void to_string(
        const HsmObject& item,
        std::string& output,
        const std::string& id = {}) const override;

    void to_string(const std::vector<HsmObject>& items, std::string& output)
        const override;

    void from_string(const std::string& output, HsmObject& item) const override;

    void from_string(
        const std::string& json, std::vector<HsmObject>& item) const override;

    bool matches_query(
        const HsmObject& item, const Metadata& query) const override
    {
        (void)item;
        (void)query;
        return true;
    };

  private:
    void to_dict(
        const HsmObject& node,
        Dictionary& dict,
        const std::string& id = {}) const;

    void from_dict(const Dictionary& dict, HsmObject& node) const;

    std::string m_internal_prefix{"_internal_"};
};

}  // namespace hestia