#pragma once

#include "ConnectionResult.h"
#include "ThreadCollection.h"

#include <condition_variable>
#include <functional>
#include <future>

class Socket;
class SocketFactory;

class TcpServer {
  public:
    struct Address {
        std::string m_prefix;
        std::string m_host;
        unsigned int m_port{8000};
    };

    using onConnectionSuccessFunc = std::function<void(Socket*)>;
    using onConnectionFailedFunc = std::function<void(const ConnectionResult&)>;

    TcpServer(std::unique_ptr<SocketFactory> socket_factory = nullptr);

    virtual ~TcpServer();

    void listen(
        const Address& address,
        onConnectionSuccessFunc connection_success_func,
        onConnectionFailedFunc connection_failed_func);

    void shut_down();

    void wait_until_bound();

  private:
    void on_connection(std::unique_ptr<Socket> client_handle);

    void on_thread_complete(std::thread::id id);

    ThreadCollection m_threads;

    std::unique_ptr<Socket> m_working_socket;
    std::future<int> m_working_socket_task;
    std::condition_variable m_working_socket_bound_cv;
    std::mutex m_working_socket_bind_mutex;

    std::unique_ptr<SocketFactory> m_socket_factory;
    onConnectionSuccessFunc m_connection_callback;
    onConnectionFailedFunc m_failed_callback;
};