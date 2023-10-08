#pragma once

#include "HttpRequest.h"

#include <string>

namespace hestia {
class S3Path {
  public:
    S3Path() = default;

    S3Path(const HttpRequest& request);

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
};
}  // namespace hestia