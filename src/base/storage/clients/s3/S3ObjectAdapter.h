#pragma once

#include "S3Bucket.h"
#include "S3Object.h"
#include "S3Request.h"
#include "StorageObject.h"

#include <memory>
#include <string>

namespace hestia {
class S3ObjectAdapter {
  public:
    static std::unique_ptr<S3ObjectAdapter> create(const std::string&);

    void to_s3(
        S3Object& s3_object, S3Request& request, const StorageObject& object);

    void from_s3(
        StorageObject& object,
        const S3Bucket& s3_container,
        const S3Object& s3_object);
};
}  // namespace hestia
