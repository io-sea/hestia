#pragma once

#include <memory>
#include <string>

namespace hestia {
class S3ObjectAdapter {
  public:
    static std::unique_ptr<S3ObjectAdapter> create(const std::string&)
    {
        return std::make_unique<S3ObjectAdapter>();
    }

    void to_s3(S3Object& s3_object, const StorageObject& object)
    {
        (void)s3_object;
        (void)object;
    }

    void from_s3(
        StorageObject& object, S3Container& s3_container, S3Object& s3_object)
    {
        (void)s3_object;
        (void)s3_container;
        (void)object;
    }
};
}  // namespace hestia