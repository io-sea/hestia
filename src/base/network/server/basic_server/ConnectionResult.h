#pragma once

#include <string>

struct ConnectionResult {
    enum class Status { OK, FAILED };

    bool ok() const { return m_status != Status::FAILED; }

    void on_error(int code, const std::string& message)
    {
        m_error_code    = code;
        m_error_message = message;
        m_status        = Status::FAILED;
    }

    Status m_status{Status::FAILED};
    std::string m_error_message;
    int m_error_code{-1};
    std::string m_body;
};