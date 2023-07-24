#pragma once

#include <string>

namespace hestia {
class IdGenerator {
  public:
    virtual ~IdGenerator()                             = default;
    virtual std::string get_id(const std::string& key) = 0;
};

class DefaultIdGenerator : public IdGenerator {
  public:
    std::string get_id(const std::string& key) override;
};
}  // namespace hestia