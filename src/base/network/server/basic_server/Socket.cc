#include "Socket.h"

#include "Logger.h"

#include <iostream>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

Socket::Ptr SocketFactory::create(const std::string& address, unsigned port)
{
    return Socket::create(address, port);
}

Socket::Socket(const std::string& address, unsigned port) :
    m_address(address), m_port(port)
{
}

Socket::Socket(int handle) : m_handle(handle)
{
    m_state.set_connected();
}

Socket::Ptr Socket::create(const std::string& address, unsigned port)
{
    return std::make_unique<Socket>(address, port);
}

Socket::Ptr Socket::create(int handle)
{
    return std::make_unique<Socket>(handle);
}

Socket::~Socket() {}

bool Socket::ok() const
{
    bool ok{false};
    {
        std::scoped_lock guard(m_mutex);
        ok = m_state.ok();
    }
    return ok;
}

bool Socket::connected() const
{
    bool connected{false};
    {
        std::scoped_lock guard(m_mutex);
        connected = m_state.connected();
    }
    return connected;
}

bool Socket::bound() const
{
    bool bound{false};
    {
        std::scoped_lock guard(m_mutex);
        bound = m_state.bound();
    }
    return bound;
}

bool Socket::closed() const
{
    bool closed{false};
    {
        std::scoped_lock guard(m_mutex);
        closed = m_state.m_connect_status == SocketState::ConnectStatus::UNSET;
    }
    return closed;
}

const SocketState& Socket::get_state() const
{
    std::scoped_lock guard(m_mutex);
    return m_state;
}

unsigned Socket::get_port() const
{
    return m_port;
}

std::string Socket::get_address() const
{
    return m_address;
}

void Socket::initialize()
{
    m_handle         = ::socket(AF_INET, SOCK_STREAM, 0);
    const int re_use = 1;
    ::setsockopt(m_handle, SOL_SOCKET, SO_REUSEADDR, &re_use, sizeof(int));
    ::setsockopt(m_handle, SOL_SOCKET, SO_REUSEPORT, &re_use, sizeof(int));
}

void Socket::do_connect()
{
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    ::inet_aton(m_address.c_str(), &serv_addr.sin_addr);
    serv_addr.sin_port = htons(m_port);

    int result = ::connect(m_handle, (sockaddr*)&serv_addr, sizeof(serv_addr));
    if (result < 0) {
        const std::string msg = "Socket: Unable to connect to server.";
        LOG_ERROR(msg);
        {
            std::scoped_lock guard(m_mutex);
            m_state.on_connect_error(result, msg);
        }
        return;
    }

    {
        std::scoped_lock guard(m_mutex);
        m_state.set_connected();
    }
}

void Socket::do_bind()
{
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(m_port);

    errno      = 0;
    int result = ::bind(m_handle, (sockaddr*)&serv_addr, sizeof(serv_addr));
    if (result < 0) {
        const std::string msg = "Socket: Unable to bind socket";
        LOG_ERROR(msg);
        {
            std::scoped_lock guard(m_mutex);
            m_state.on_bind_error(result, msg);
        }
        return;
    }

    {
        std::scoped_lock guard(m_mutex);
        m_state.set_bound();
    }
}

void Socket::do_listen(
    onIncomingConnectionFunc connection_func, onBoundFunc bound_func)
{
    initialize();
    if (!ok()) {
        bound_func(false);
        return;
    }

    do_bind();
    if (!ok()) {
        bound_func(false);
        return;
    }

    listen_impl();

    bound_func(true);

    while (bound()) {
        const auto new_socket_handle = do_accept();
        if (new_socket_handle < 0) {
            if (!closed()) {
                const std::string msg = "Socket: Accept failed";
                {
                    std::scoped_lock guard(m_mutex);
                    m_state.on_bind_error(new_socket_handle, msg);
                }
                LOG_ERROR(msg);
                do_close();
            }
            return;
        }
        connection_func(new_socket_handle);
    }
}

std::string Socket::send(const std::string& message)
{
    if (!m_state.connected()) {
        initialize();
        if (!ok()) {
            return {};
        }

        do_connect();
        if (!ok()) {
            return {};
        }
    }

    auto n = ::write(m_handle, message.c_str(), message.length());
    if (n < 0) {
        on_socker_error("Socket: Send failed.");
        return {};
    }

    std::string response;
    while (m_state.connected()) {
        response += recieve();
    }
    do_close();

    return response;
}

void Socket::on_socker_error(const std::string& message)
{
    LOG_ERROR(message);

    bool connect_ok{false};
    {
        std::scoped_lock guard(m_mutex);
        m_state.on_connect_error(-1, message);
        connect_ok = m_state.m_connect_status == SocketState::ConnectStatus::OK;
    }

    if (connect_ok) {
        do_close();
    }

    {
        std::scoped_lock guard(m_mutex);
        m_state.m_connect_status = SocketState::ConnectStatus::FAILED;
    }
}

std::string Socket::recieve()
{
    const int buffer_size = 512;
    char buffer[buffer_size];
    auto result = ::read(m_handle, buffer, buffer_size);
    if (result > 0) {
        return std::string(buffer);
    }
    else if (result == 0) {
        {
            std::scoped_lock guard(m_mutex);
            m_state.set_disconnected();
        }
    }
    else {
        const std::string msg = "Socket: read failed.";
        LOG_ERROR(msg);
        {
            std::scoped_lock guard(m_mutex);
            m_state.on_connect_error(result, msg);
        }
    }
    return {};
}

void Socket::close()
{
    LOG_INFO("Closing Socket");
    do_close();
    {
        std::scoped_lock guard(m_mutex);
        m_state.set_disconnected();
    }
}

void Socket::respond(const std::string& message)
{
    auto result =
        ::write(m_handle, message.c_str(), static_cast<int>(message.size()));
    (void)result;
}

void Socket::listen_impl()
{
    ::listen(m_handle, 5);
}

int Socket::do_accept()
{
    sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);
    return ::accept(m_handle, (sockaddr*)&cli_addr, &clilen);
}

void Socket::do_close()
{
    ::close(m_handle);
}