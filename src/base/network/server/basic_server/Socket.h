#pragma once

#include "SocketState.h"

#include <functional>
#include <memory>
#include <mutex>

/**
 * @brief Socket class - wraps the system socket library
 *
 * This class wraps the system socket library - providing an interface for
 * working with sockets.
 */
class Socket {
  public:
    using Ptr = std::unique_ptr<Socket>;

    /**
     * Constructor
     *
     * @param address Network address to listen on
     * @param port Port to listen on
     */
    Socket(const std::string& address, unsigned port);

    /**
     * Constructor
     *
     * @param handle A system socket handle
     */
    Socket(int handle);

    /**
     * Factory Constructor
     *
     * @param address Network address to listen on
     * @param port Port to listen on
     * @return a ptr to a new socket
     */
    static Ptr create(const std::string& address, unsigned port);

    /**
     * Factory Constructor
     *
     * @param handle A system socket handle
     * @return a ptr to a new socket
     */
    static Ptr create(int handle);

    virtual ~Socket();

    /**
     * True if the socket is connected to another end
     *
     * @return True if the socket is connected to another end
     */
    bool connected() const;

    /**
     * True if the socket is bound as a server
     *
     * @return True if the socket is bound as a server
     */
    bool bound() const;

    /**
     * True if the socket is in a closed state
     *
     * @return True if the socket is in a closed state
     */
    bool closed() const;

    /**
     * Close the socket
     */
    void close();

    /**
     * Start listening at the previously supplied address and port
     * Call the input func for any new connections
     * @param connection_func function to be called for any new connections
     */
    using onIncomingConnectionFunc = std::function<void(int)>;
    using onBoundFunc              = std::function<void(bool)>;
    void do_listen(
        onIncomingConnectionFunc connection_func, onBoundFunc bound_func);

    /**
     * Return the network address of the socket
     * @return the network address of the socket
     */
    std::string get_address() const;

    /**
     * Return the port of the socket
     * @return the port of the socket
     */
    unsigned get_port() const;

    /**
     * Return the socket's state
     * @return the socket state
     */
    const SocketState& get_state() const;

    /**
     * True if the socket is in a non-error state
     * @return True if the socket is in a non-error state
     */
    bool ok() const;

    /**
     * Wait on the connection to receive content
     * @return the received content
     */
    std::string recieve();

    /**
     * Respond to a received message
     * @param message the response
     */
    void respond(const std::string& message);

    /**
     * Send a message
     * @param message the message
     * @return the response
     */
    std::string send(const std::string& message);

  protected:
    virtual void initialize();

    void on_socker_error(const std::string& message);

    /**
     * This and subsequent methods allow overriding for mocks in tests.
     */
    virtual void do_connect();

    virtual void do_bind();

    virtual void do_close();

    virtual void listen_impl();

    virtual int do_accept();

    int m_handle{0};

    mutable std::mutex m_mutex;
    SocketState m_state;

    unsigned m_port{0};
    std::string m_address;
};

class SocketFactory {
  public:
    virtual Socket::Ptr create(const std::string& address, unsigned port);
};