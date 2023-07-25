#pragma once

#ifdef HAVE_PROXYGEN

#include "ProxygenServer.h"

#include <folly/init/Init.h>

#include <memory>

using TestServer = hestia::ProxygenServer;

class ProxygenTestContext {
  public:
    void do_initialize()
    {
        int folly_argc = 0;
        folly::InitOptions init_options;
        init_options.remove_flags = false;
        init_options.use_gflags   = false;
        m_folly_instance =
            std::make_unique<folly::Init>(&folly_argc, nullptr, init_options);
    }

  private:
    std::unique_ptr<folly::Init> m_folly_instance;
};

#else

#include "BasicHttpServer.h"
using TestServer = hestia::BasicHttpServer;

class ProxygenTestContext {
  public:
    void do_initialize() {}
};
#endif