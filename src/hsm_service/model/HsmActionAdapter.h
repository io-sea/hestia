#pragma once

#include "HsmAction.h"

#include <memory>

namespace hestia {
class HsmActionAdapter {
  public:
    using Ptr = std::unique_ptr<HsmActionAdapter>;

    static Ptr create();

    void parse(const std::string& input, HsmAction& action);
};
}  // namespace hestia