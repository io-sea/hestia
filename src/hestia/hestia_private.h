#pragma once

#include "HestiaClient.h"

namespace hestia {
class HestiaPrivate {
  public:
    static void override_client(std::unique_ptr<IHestiaClient> client);

    static IHestiaClient* get_client();

    static bool check_initialized();

    static int hestia_start_server(
        const char* host, int port, const char* config);

    static int hestia_stop_server();
};

}  // namespace hestia