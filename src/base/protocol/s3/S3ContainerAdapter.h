#pragma once

#include "S3Container.h"
#include "StorageObject.h"

#include <memory>

namespace hestia {
class S3ContainerAdapter {
  public:
    using Ptr = std::unique_ptr<S3ContainerAdapter>;

    S3ContainerAdapter(const std::string& metadata_prefix);

    static Ptr create(const std::string& metadata_prefix);

    virtual ~S3ContainerAdapter() = default;

    virtual void from_s3(
        StorageObject& storage_object, const S3Container& container);

    virtual void to_s3(
        S3Container& container, const StorageObject& storage_object);

    Metadata::Query get_query_filter() const;

  private:
    std::string m_metadata_prefix;
};
}  // namespace hestia