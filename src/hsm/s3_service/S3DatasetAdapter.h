#pragma once

#include "Dataset.h"
#include "StringAdapter.h"

#include <memory>

namespace hestia {
class S3DatasetAdapter : public StringAdapter<Dataset> {
  public:
    using Ptr = std::unique_ptr<S3DatasetAdapter>;

    S3DatasetAdapter(const std::string& metadata_prefix = {});

    static Ptr create(const std::string& metadata_prefix = {});

    virtual ~S3DatasetAdapter() = default;

    Dictionary::Ptr dict_from_string(const std::string& input) const override;

    void dict_to_string(
        const Dictionary& dict, std::string& output) const override;

    void on_list(const Dataset& dataset, std::string& output);

    void on_list(const std::vector<Dataset>& dataset, std::string& output);

  private:
    std::string m_metadata_prefix;
};
}  // namespace hestia