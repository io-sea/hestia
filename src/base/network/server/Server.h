#pragma once

#include "WebApp.h"

#include <memory>
#include <mutex>
#include <string>

namespace hestia {
class Server {
  public:
    using Ptr = std::unique_ptr<Server>;

    struct Config {
        std::string m_ip{"127.0.0.1"};
        int m_http_port{8000};
        int m_http2_port{8080};
        std::size_t m_num_threads{0};
        bool m_block_on_launch{false};
        int m_argc{0};
        char** m_argv{nullptr};
    };

    enum class Status {
        UNINITLAIZED,
        INITIALIZE_FAILED,
        START_FAILED,
        RUNNING,
        STOP_REQUESTED
    };

    Server(const Config& config, WebApp* web_app);

    virtual ~Server() = default;

    static Ptr create(const Config& config, WebApp* web_app);

    virtual Status initialize();

    virtual Status start();

    virtual Status stop();

  protected:
    Status get_status() const;

    void set_status(Status status);

    WebApp* m_web_app{nullptr};
    Config m_config;

    mutable std::mutex m_mutex;
    Status m_status{Status::UNINITLAIZED};
};
}  // namespace hestia