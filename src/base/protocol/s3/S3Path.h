#pragma once

#include "HttpRequest.h"

#include <string>

namespace hestia {
class S3Path {
  public:
    S3Path(const HttpRequest&);

    S3Path(const std::string&);

    std::string m_container_name;
    std::string m_object_id;

    static constexpr char meta_prefix[] = "x-amz-meta-";

  private:
    void from_path_only(const std::string& path);
};
}  // namespace hestia