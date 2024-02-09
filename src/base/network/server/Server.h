#pragma once

#include "WebApp.h"

#include <memory>
#include <mutex>
#include <string>

namespace hestia {

struct BaseServerConfig {
    std::string m_ip{"127.0.0.1"};
    std::string m_tag;
    int m_http_port{8000};
    int m_http2_port{8080};
    std::size_t m_num_threads{0};
    std::size_t m_body_buffer_size{0x77359400};  // Approx 2GB
    bool m_block_on_launch{false};
    int m_argc{0};
    char** m_argv{nullptr};
};

class Server {
  public:
    using Ptr = std::unique_ptr<Server>;

    enum class Status {
        UNINITLAIZED,
        INITIALIZE_FAILED,
        START_FAILED,
        RUNNING,
        STOP_REQUESTED
    };

    Server(const BaseServerConfig& config, WebApp* web_app);

    virtual ~Server() = default;

    static Ptr create(const BaseServerConfig& config, WebApp* web_app);

    const BaseServerConfig& get_config() const { return m_config; }

    virtual Status initialize();

    virtual Status start();

    virtual Status stop();

    virtual void wait_until_bound();

  protected:
    Status get_status() const;

    void set_status(Status status);

    WebApp* m_web_app{nullptr};
    BaseServerConfig m_config;

    mutable std::mutex m_mutex;
    Status m_status{Status::UNINITLAIZED};
};
}  // namespace hestia