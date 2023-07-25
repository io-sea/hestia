#pragma once

#include "HestiaClient.h"

namespace hestia {
class HestiaPrivate {
  public:
    static void override_client(std::unique_ptr<IHestiaClient> client);
};

}  // namespace hestia