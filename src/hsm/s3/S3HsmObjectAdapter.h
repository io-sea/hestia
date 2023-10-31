#pragma once

#include "Dataset.h"
#include "HsmObject.h"
#include "S3Responses.h"

#include <memory>

namespace hestia {
class S3HsmObjectAdapter {
  public:
    using Ptr = std::unique_ptr<S3HsmObjectAdapter>;

    S3HsmObjectAdapter(const std::string& metadata_prefix = {});

    static Ptr create(const std::string& metadata_prefix = {});

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