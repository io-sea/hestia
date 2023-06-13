#pragma once

#include "StorageTier.h"
#include "StringAdapter.h"

#include "Dictionary.h"

namespace hestia {
class StorageTierJsonAdapter : public StringAdapter<StorageTier> {
  public:
    void to_string(
        const StorageTier& item,
        std::string& output,
        const std::string& id = {}) const override;

    void to_string(const std::vector<StorageTier>& items, std::string& output)
        const override;

    void from_string(
        const std::string& output, StorageTier& item) const override;

    void from_string(const std::string& json, std::vector<StorageTier>& items)
        const override;

    bool matches_query(
        const StorageTier& item, const Metadata& query) const override
    {
        (void)item;
        (void)query;
        return true;
    };

  private:
    void to_dict(
        const StorageTier& item,
        Dictionary& dict,
        const std::string& id = {}) const;

    void from_dict(const Dictionary& dict, StorageTier& item) const;
};

}  // namespace hestia