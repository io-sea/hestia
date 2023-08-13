#include "ProxygenServer.h"

#ifdef HAVE_PROXYGEN
#include <folly/executors/CPUThreadPoolExecutor.h>
#include <folly/executors/GlobalExecutor.h>

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

ProxygenServer::~ProxygenServer()
{
    ProxygenServer::stop();
}

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

    proxygen::HTTPServerOptions options;
    // options.threads = mConfig.mNumThreads;
    options.idleTimeout              = std::chrono::milliseconds(60000);
    options.shutdownOn               = {SIGINT, SIGTERM};
    options.enableContentCompression = false;
    options.handlerFactories         = proxygen::RequestHandlerChain()
                                   .addThen<ProxygenHandlerFactory>(
                                       m_web_app, m_config.m_body_buffer_size)
                                   .build();
    options.h2cEnabled = true;

    auto io_thread_pool = std::make_shared<folly::CPUThreadPoolExecutor>(
        m_config.m_num_threads, 1,
        std::make_shared<folly::NamedThreadFactory>("FIFOIOThread"));
    // TODO: It would be better to use getGlobalCPUExecutor
    folly::setUnsafeMutableGlobalCPUExecutor(io_thread_pool);

    m_impl = std::make_unique<proxygen::HTTPServer>(std::move(options));
    m_impl->bind(ip_addresses);

    return {};
}

ProxygenServer::Status ProxygenServer::start()
{
    auto worker = std::make_unique<std::thread>([this]() {
        LOG_INFO(
            "About to start server, impl is: " + std::to_string(bool(m_impl)));
        m_impl->start([this]() {
            std::unique_lock<std::mutex> lck(m_bound_mutex);
            m_bound_cv.notify_all();
        });
    });

    m_threads.add(std::move(worker));
    if (m_config.m_block_on_launch) {
        m_threads.join_and_clear_all();
    }
    return {};
}

void ProxygenServer::wait_until_bound()
{
    std::unique_lock<std::mutex> lck(m_bound_mutex);
    m_bound_cv.wait(lck);
    LOG_INFO("Server is bound, impl is: " + std::to_string(bool(m_impl)));
}

ProxygenServer::Status ProxygenServer::stop()
{
    if (m_threads.size() > 0) {
        LOG_INFO("Stopping server, impl is: " + std::to_string(bool(m_impl)));
        m_impl->stop();
        m_threads.join_and_clear_all();
    }
    return {};
}


}  // namespace hestia
#endif