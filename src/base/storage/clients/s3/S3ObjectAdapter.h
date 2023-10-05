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
    static std::unique_ptr<S3ObjectAdapter> create(const std::string&)
    {
        return std::make_unique<S3ObjectAdapter>();
    }

    void to_s3(
        S3Object& s3_object, S3Request& request, const StorageObject& object)
    {
        for (const auto& [key, value] : object.metadata().data()) {
            if (key == "hestia-bucket_name") {
                s3_object.m_bucket = value;
            }
            else if (key == "hestia-user_id") {
                request.m_user_context.m_user_id = value;
            }
            else if (key == "hestia-user_token") {
                request.m_user_context.m_user_secret_key = value;
            }
            else if (key == "hestia-s3_region") {
                request.m_region = value;
            }
            else {
                s3_object.metadata.set_item(key, value);
            }
            s3_object.m_key = object.get_primary_key();
        }
    }

    void from_s3(
        StorageObject& object, S3Bucket& s3_container, S3Object& s3_object)
    {
        (void)s3_object;
        (void)s3_container;
        (void)object;
    }
};
}  // namespace hestia