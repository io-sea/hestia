#pragma once

#include <string>

namespace hestia {
class UserTokenGenerator {
  public:
    virtual ~UserTokenGenerator();
    virtual std::string generate(const std::string& key = {}) const;
};
}  // namespace hestia