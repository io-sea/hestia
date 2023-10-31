#pragma once

#include "Dataset.h"
#include "S3Responses.h"

#include <memory>

namespace hestia {
class S3DatasetAdapter {
  public:
    using Ptr = std::unique_ptr<S3DatasetAdapter>;

    S3DatasetAdapter(const std::string& metadata_prefix = {});

    static Ptr create(const std::string& metadata_prefix = {});

    virtual ~S3DatasetAdapter() = default;

    void on_list_buckets(
        const VecModelPtr& items, S3ListBucketResponse& list_bucket_response);

  private:
    std::string m_metadata_prefix;
};
}  // namespace hestia