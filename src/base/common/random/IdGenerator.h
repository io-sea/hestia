#pragma once

#include <string>

#include "Uuid.h"

namespace hestia {
class IdGenerator {
  public:
    virtual ~IdGenerator()                        = default;
    virtual Uuid get_uuid(const std::string& key) = 0;
};

class DefaultIdGenerator : public IdGenerator {
  public:
    Uuid get_uuid(const std::string& key) override;
};
}  // namespace hestia