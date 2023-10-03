#pragma once

#include "HttpRequest.h"

#include <string>

namespace hestia {
class S3Path {
  public:
    S3Path(const HttpRequest& request);

    S3Path(const std::string& path);

    std::string m_container_name;
    std::string m_object_id;
    std::string m_queries;

    static constexpr char meta_prefix[] = "x-amz-meta-";

  private:
    void from_path_only(const std::string& path);
};
}  // namespace hestia