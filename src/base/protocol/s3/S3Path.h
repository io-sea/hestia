#pragma once

#include "HttpRequest.h"

#include <string>

namespace hestia {

/*
This attempts to handle both PATH and VIRTUAL-HOST style S3 addressing.

The specification from AWS is not comprehensive so we are taking a best shot at
interpreting:
https://docs.aws.amazon.com/AmazonS3/latest/userguide/VirtualHosting.html

The scenarios are:

* The HOST header is empty (HTTP 1.0 only) or its value is MY_DOMAIN
(s3.region-code.amazonaws.com for AWS) - the URI has both bucket and object key.
* The HOST header is of form 'xyz.MY_DOMAIN' - the bucket is taken as 'xyz' and
the URI is the object key.
* The bucket is taken as HOST and the URI is the object key.
*/

class S3Path {
  public:
    S3Path() = default;

    S3Path(const HttpRequest& request, const std::string& domain);

    S3Path(const std::string& path);

    std::string get_resource_path() const;

    bool is_same_resource(
        const std::string& bucket_name, const std::string& object_key) const
    {
        return m_bucket_name == bucket_name && m_object_key == object_key;
    }

    std::string m_bucket_name;
    std::string m_object_key;
    std::string m_queries;

    static constexpr char meta_prefix[] = "x-amz-meta-";

  private:
    void from_path_only(const std::string& path);
    void object_from_path_only(const std::string& path);
};
}  // namespace hestia