#include <catch2/catch_all.hpp>

#include "Socket.h"
#include "TcpServer.h"

#include <condition_variable>
#include <future>
#include <iostream>

class TestSocket : public Socket {
  public:
    TestSocket(const std::string& address, unsigned port) :
        Socket(address, port)
    {
    }

    void do_bind() override
    {
        m_state.set_bound();

        std::unique_lock<std::mutex> lck(mtx);
        cv.notify_one();
    }

    void do_close() override { m_state.set_disconnected(); }

    void listen_impl() override {}

    int do_accept() override { return 0; }

    void wait_until_bound()
    {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, [this]() { return bound(); });
    }

    std::condition_variable cv;
    std::mutex mtx;
};

class TestSocketFactory : public SocketFactory {
  public:
    Socket::Ptr create(const std::string& address, unsigned port)
    {
        return std::make_unique<TestSocket>(address, port);
    }
};

TEST_CASE("Test TcpServer", "[server]")
{
    auto socket_factory = std::make_unique<TestSocketFactory>();

    TcpServer server(std::move(socket_factory));

    auto on_connect_func = [](Socket* socket) {};

    auto on_connect_failed_func = [](const ConnectionResult& result) {};

    TcpServer::Address address;
    address.m_host = "0.0.0.0";
    address.m_port = 8080;
    server.listen(address, on_connect_func, on_connect_failed_func);
    server.wait_until_bound();
    server.shut_down();
};