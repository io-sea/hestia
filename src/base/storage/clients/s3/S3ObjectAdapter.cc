#include "S3ObjectAdapter.h"

#include <iostream>

namespace hestia {
std::unique_ptr<S3ObjectAdapter> S3ObjectAdapter::create(const std::string&)
{
    return std::make_unique<S3ObjectAdapter>();
}

void S3ObjectAdapter::to_s3(
    S3Object& s3_object, S3Request& request, const StorageObject& object)
{
    for (const auto& [key, value] : object.metadata().data()) {
        if (key == "hestia-bucket_name") {
            s3_object.m_bucket = value;
        }
        else if (key == "hestia-user_id") {
            request.set_user_id(value);
        }
        else if (key == "hestia-user_token") {
            request.set_user_secret_key(value);
        }
        else if (key == "hestia-s3_region") {
            request.m_region = value;
        }
        else {
            s3_object.metadata.set_item(key, value);
        }
        s3_object.m_key = object.get_primary_key();
    }
    s3_object.m_size = object.size();
}

void S3ObjectAdapter::from_s3(
    StorageObject& object,
    const S3Bucket& s3_container,
    const S3Object& s3_object)
{
    (void)s3_object;
    (void)s3_container;
    (void)object;
}
}  // namespace hestia