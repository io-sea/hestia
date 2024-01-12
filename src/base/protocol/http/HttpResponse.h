#pragma once

#include "HttpHeader.h"
#include "HttpStatus.h"

#include <memory>
#include <sstream>

namespace hestia {
class HttpResponse {
  public:
    using Ptr = std::unique_ptr<HttpResponse>;

    enum class CompletionStatus { FINISHED, AWAITING_EOM, AWAITING_BODY_CHUNK };

    HttpResponse() = default;
    HttpResponse(
        int code, const std::string& message, const std::string& body = {});
    HttpResponse(const HttpStatus& status);
    HttpResponse(CompletionStatus status);

    static Ptr create();
    static Ptr create(
        int code, const std::string& message, const std::string& body = {});

    static Ptr create(const HttpStatus& status);
    static Ptr create(CompletionStatus status);

    bool error() const { return m_code >= 400; }

    int code() const;

    CompletionStatus get_completion_status() const
    {
        return m_completion_status;
    }

    void set_completion_status(CompletionStatus status)
    {
        m_completion_status = status;
    }

    std::string get_error_message() const 
    {
        auto msg = std::to_string(m_code) + " | " + m_message;
        if (!m_body.empty())
        {
            msg += " | " + m_body;
        }
        return msg;
    }

    const std::string& message() const;

    const std::string& body() const;

    std::string& body() { return m_body; }

    HttpHeader& header();

    const HttpHeader& header() const;

    void append_to_body(const std::string& body);

    void set_body(const std::string& body);

    std::string to_string() const;

  private:
    int m_code{200};
    CompletionStatus m_completion_status{CompletionStatus::FINISHED};
    std::string m_message{"OK"};
    std::string m_body;
    HttpHeader m_header;
};
}  // namespace hestia