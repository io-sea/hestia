#pragma once

#include "Dataset.h"
#include "HsmObject.h"
#include "StringAdapter.h"

#include <memory>

namespace hestia {
class S3ObjectAdapter : public StringAdapter<HsmObject> {
  public:
    using Ptr = std::unique_ptr<S3ObjectAdapter>;

    S3ObjectAdapter(const std::string& metadata_prefix = {});

    static Ptr create(const std::string& metadata_prefix = {});

    Dictionary::Ptr dict_from_string(const std::string& input) const override;

    void dict_to_string(
        const Dictionary& dict, std::string& output) const override;

    void get_headers(
        const Dataset& dataset, const HsmObject& object, Metadata& header);

  private:
    std::string m_metadata_prefix;
};
}  // namespace hestia