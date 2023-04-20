#pragma once

#include "HsmAction.h"

#include <memory>

class HsmActionAdapter {
  public:
    using Ptr = std::unique_ptr<HsmActionAdapter>;

    static Ptr Create();

    void parse(const std::string& input, HsmAction& action);
};