#include "Server.h"

#include "BasicHttpServer.h"

namespace hestia {

std::unique_ptr<Server> Server::create(
    const BaseServerConfig& config, WebApp* web_app)
{
    return std::make_unique<BasicHttpServer>(config, web_app);
}

Server::Server(const BaseServerConfig& config, WebApp* web_app) :
    m_web_app(web_app), m_config(config)
{
}

Server::Status Server::initialize()
{
    return {};
}

Server::Status Server::start()
{
    return {};
}

Server::Status Server::stop()
{
    return {};
}

void Server::wait_until_bound()
{
    return;
}

Server::Status Server::get_status() const
{
    std::scoped_lock guard(m_mutex);
    return m_status;
}

void Server::set_status(Status status)
{
    std::scoped_lock guard(m_mutex);
    m_status = status;
}

}  // namespace hestia