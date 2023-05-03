#include "ProxygenServer.h"

#ifdef HAVE_PROXYGEN
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/executors/GlobalExecutor.h>
#include <folly/init/Init.h>
#include <folly/portability/GFlags.h>
#include <proxygen/httpserver/HTTPServer.h>
#include <proxygen/httpserver/RequestHandlerFactory.h>

#include "Logger.h"
#include "ProxygenHandlerFactory.h"

using Protocol  = proxygen::HTTPServer::Protocol;
using IpAddress = proxygen::HTTPServer::IPConfig;

namespace hestia {
ProxygenServer::ProxygenServer(const Server::Config& config, WebApp* web_app) :
    Server(config, web_app)
{
}

ProxygenServer::~ProxygenServer() {}

ProxygenServer::Status ProxygenServer::initialize()
{
    std::vector<IpAddress> ip_addresses;
    try {
        ip_addresses = {
            {folly::SocketAddress(m_config.m_ip, m_config.m_http_port, true),
             Protocol::HTTP},
            {folly::SocketAddress(m_config.m_ip, m_config.m_http2_port, true),
             Protocol::HTTP2},
        };
    }
    catch (const std::system_error& ex) {
        std::cerr << "deimos: " << ex.what() << '\n';
        return ProxygenServer::Status::INITIALIZE_FAILED;
    }

    // return {};
    int folly_argc = 0;

    folly::InitOptions init_options;
    init_options.remove_flags = false;
    init_options.use_gflags   = false;

    folly::init(&folly_argc, nullptr, init_options);

    proxygen::HTTPServerOptions options;
    // options.threads = mConfig.mNumThreads;
    options.idleTimeout              = std::chrono::milliseconds(60000);
    options.shutdownOn               = {SIGINT, SIGTERM};
    options.enableContentCompression = false;
    options.handlerFactories         = proxygen::RequestHandlerChain()
                                   .addThen<ProxygenHandlerFactory>(m_web_app)
                                   .build();
    options.h2cEnabled = true;

    auto io_thread_pool = std::make_shared<folly::CPUThreadPoolExecutor>(
        m_config.m_num_threads, 0,
        std::make_shared<folly::NamedThreadFactory>("FIFOIOThread"));
    // TODO: It would be better to use getGlobalCPUExecutor
    folly::setUnsafeMutableGlobalCPUExecutor(io_thread_pool);

    m_impl = std::make_unique<proxygen::HTTPServer>(std::move(options));
    m_impl->bind(ip_addresses);

    return {};
}

ProxygenServer::Status ProxygenServer::start()
{
    std::thread t([this]() {
        LOG_INFO(
            "About to start server, impl is: " + std::to_string(bool(m_impl)));
        m_impl->start();
    });

    t.join();
    return {};
}
}  // namespace hestia
#endif