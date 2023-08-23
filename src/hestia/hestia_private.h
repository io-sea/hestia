#pragma once

#include "HestiaClient.h"

namespace hestia {
class HestiaPrivate {
  public:
    static void override_client(std::unique_ptr<IHestiaClient> client);

    static int hestia_start_server(
        const char* host, int port, const char* config);

    static int hestia_stop_server();
};

}  // namespace hestia