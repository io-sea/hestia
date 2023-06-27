#pragma once

#include <memory>
#include <string>

namespace hestia {
class S3ContainerAdapter {
  public:
    static std::unique_ptr<S3ContainerAdapter> create(const std::string&)
    {
        return std::make_unique<S3ContainerAdapter>();
    }
};
}  // namespace hestia