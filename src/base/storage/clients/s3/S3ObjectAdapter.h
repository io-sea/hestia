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
        for (const auto& [key, value] : object.metadata().data()) {
            if (key == "hestia-bucket_name") {
                s3_object.m_container = value;
            }
            else if (key == "hestia-user_id") {
                s3_object.m_user = value;
            }
            else if (key == "hestia-user_token") {
                s3_object.m_user_token = value;
            }
            else if (key == "hestia-s3_region") {
                s3_object.m_region = value;
            }
            else {
                s3_object.metadata.set_item(key, value);
            }
            s3_object.m_name = object.get_primary_key();
        }
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