#include "TcpServer.h"

#include "Logger.h"
#include "Socket.h"

TcpServer::TcpServer(std::unique_ptr<SocketFactory> socket_factory) :
    m_socket_factory(
        socket_factory ? std::move(socket_factory) :
                         std::make_unique<SocketFactory>())
{
}

TcpServer::~TcpServer()
{
    shut_down();
}

void TcpServer::listen(
    const Address& address,
    onConnectionSuccessFunc connection_success_func,
    onConnectionFailedFunc connection_failed_func)
{
    m_connection_callback = connection_success_func;
    m_failed_callback     = connection_failed_func;

    m_working_socket = m_socket_factory->create(address.m_host, address.m_port);

    auto socket_task = [this]() {
        LOG_INFO("Launching server bind socket");
        auto on_connection = [this](int handle) {
            this->on_connection(Socket::create(handle));
        };

        auto on_bound = [this](bool bound) {
            (void)bound;
            LOG_INFO("Socket bound");
            std::unique_lock<std::mutex> lck(m_working_socket_bind_mutex);
            m_working_socket_bound_cv.notify_one();
        };

        m_working_socket->do_listen(on_connection, on_bound);

        if (!m_working_socket->closed() && !m_working_socket->ok()) {
            ConnectionResult result;
            result.on_error(
                m_working_socket->get_state().m_error_code,
                m_working_socket->get_state().m_error_message);
            m_failed_callback(result);
        }
        return 0;
    };
    m_working_socket_task = std::async(std::launch::async, socket_task);
}

void TcpServer::shut_down()
{
    if (m_working_socket) {
        m_working_socket->close();
        LOG_INFO("Waiting for Socket to finish");
        if (m_working_socket_task.valid()) {
            m_working_socket_task.get();
        }
        m_working_socket.reset();
    }
    m_threads.remove_marked();
    m_threads.join_and_clear_all();
    LOG_INFO("Finished shutdown");
}

void TcpServer::on_connection(Socket::Ptr client_connection)
{
    m_threads.remove_marked();

    auto worker_func = [this](Socket::Ptr client_connection) {
        m_connection_callback(client_connection.get());
        this->on_thread_complete(std::this_thread::get_id());
    };

    auto worker = std::make_unique<std::thread>(
        worker_func, std::move(client_connection));
    m_threads.add(std::move(worker));
}

void TcpServer::on_thread_complete(std::thread::id id)
{
    m_threads.mark_for_removal(id);
}

void TcpServer::wait_until_bound()
{
    std::unique_lock<std::mutex> lck(m_working_socket_bind_mutex);
    m_working_socket_bound_cv.wait(lck, [this]() {
        return m_working_socket->bound() || !m_working_socket->ok();
    });
}