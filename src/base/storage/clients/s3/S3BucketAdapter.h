#pragma once

#include <memory>
#include <string>

namespace hestia {
class S3BucketAdapter {
  public:
    static std::unique_ptr<S3BucketAdapter> create(const std::string&);
};
}  // namespace hestia