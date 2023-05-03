#pragma once

#include "Request.h"
#include "RequestError.h"

namespace hestia {
template<typename ErrorCode>
class Response {
  public:
    enum class State { OK, ERROR };

    Response(const BaseRequest& request) : m_request_id(request.get_id()) {}

    virtual ~Response() = default;

    const RequestError<ErrorCode>& get_error() const { return m_error; }

    virtual bool ok() const { return m_state == State::OK; }

    void on_error(const RequestError<ErrorCode>& error)
    {
        m_error = error;
        m_state = State::ERROR;
    }

  protected:
    std::string m_request_id;
    State m_state{State::OK};
    RequestError<ErrorCode> m_error;
};
}  // namespace hestia