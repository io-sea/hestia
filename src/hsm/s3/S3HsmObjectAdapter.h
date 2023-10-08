#pragma once

#include "Dataset.h"
#include "HsmObject.h"
#include "S3Responses.h"
#include "StringAdapter.h"

#include <memory>

namespace hestia {
class S3HsmObjectAdapter : public StringAdapter {
  public:
    using Ptr = std::unique_ptr<S3HsmObjectAdapter>;

    S3HsmObjectAdapter(const std::string& metadata_prefix = {});

    static Ptr create(const std::string& metadata_prefix = {});

    void dict_from_string(
        const std::string& input,
        Dictionary& dict,
        const std::string& key_prefix = {}) const override;

    void dict_to_string(
        const Dictionary& dict, std::string& output) const override;

    void get_headers(
        const std::string& bucket_name,
        const HsmObject& object,
        Map& header) const;

    void on_list_objects(
        const Dataset& dataset, S3ListObjectsResponse& response);

  private:
    std::string m_metadata_prefix;
};
}  // namespace hestia