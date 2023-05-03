#pragma once

#include <string>

struct SocketState {
    enum class ConnectStatus { UNSET, OK, FAILED };

    enum class BindStatus { UNSET, OK, FAILED };

    bool ok() const;

    bool bound() const;

    bool connected() const;

    void on_connect_error(int code, const std::string& msg);

    void on_bind_error(int code, const std::string& msg);

    void set_disconnected();

    void set_connected();

    void set_bound();

    ConnectStatus m_connect_status{ConnectStatus::UNSET};
    BindStatus m_bind_status{BindStatus::UNSET};
    std::string m_error_message;
    int m_error_code{0};
    std::string m_body;
};