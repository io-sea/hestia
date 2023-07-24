#pragma once

#include "Dictionary.h"

namespace hestia {
class TestClientConfigs {
  public:
    static void get_hsm_memory_client_config(Dictionary& config);
};
}  // namespace hestia