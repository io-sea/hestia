#include <catch2/catch_all.hpp>

#include "Socket.h"

#include <condition_variable>
#include <future>
#include <iostream>

class TestSocket : public Socket {
  public:
    TestSocket(const std::string& address, unsigned port) :
        Socket(address, port)
    {
    }

    virtual ~TestSocket() = default;

    void do_bind() override
    {
        std::unique_lock<std::mutex> lck(mtx);
        m_state.set_bound();
        cv.notify_one();
    }

    void do_close() override {}

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

TEST_CASE("Test Socket", "[socket]")
{
    auto socket = std::make_unique<TestSocket>("0.0.0.0", 8080);

    auto socket_loop = [&socket]() {
        auto connect_func = [](int handle) { (void)handle; };

        auto bound_func = [](bool status) { (void)status; };

        socket->do_listen(connect_func, bound_func);
        return 0;
    };

    auto socket_future = std::async(std::launch::async, socket_loop);
    socket->wait_until_bound();

    socket->close();
    socket_future.get();
}