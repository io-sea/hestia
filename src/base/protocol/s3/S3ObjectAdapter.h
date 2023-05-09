#pragma once

#include "S3Container.h"
#include "S3Object.h"
#include "StorageObject.h"

#include <memory>

namespace hestia {
class S3ObjectAdapter {
  public:
    using Ptr = std::unique_ptr<S3ObjectAdapter>;

    S3ObjectAdapter(const std::string& metadata_prefix);

    static Ptr create(const std::string& metadata_prefix);

    virtual ~S3ObjectAdapter() = default;

    virtual void to_s3(S3Object& object, const StorageObject& storage_object);

    virtual void from_s3(
        StorageObject& storage_object,
        const S3Container& container,
        const S3Object& object);

    std::string get_query_filter() const;

  private:
    std::string m_metadata_prefix;
};
}  // namespace hestia