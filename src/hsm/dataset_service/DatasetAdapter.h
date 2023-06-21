#pragma once

#include "Dataset.h"
#include "StringAdapter.h"

#include "Dictionary.h"

namespace hestia {
class DatasetJsonAdapter : public StringAdapter<Dataset> {
  public:
    void to_string(
        const Dataset& item,
        std::string& output,
        const std::string& id = {}) const override;

    void to_string(
        const std::vector<Dataset>& items, std::string& output) const override;

    void from_string(const std::string& output, Dataset& item) const override;

    void from_string(
        const std::string& json, std::vector<Dataset>& items) const override;

    bool matches_query(
        const Dataset& item, const Metadata& query) const override
    {
        (void)item;
        (void)query;
        return true;
    };

  private:
    void to_dict(
        const Dataset& item,
        Dictionary& dict,
        const std::string& id = {}) const;

    void from_dict(const Dictionary& dict, Dataset& item) const;
};

}  // namespace hestia