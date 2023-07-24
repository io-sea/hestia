#pragma once

#include "Request.h"
#include "RequestError.h"

#include <memory>
#include <vector>

namespace hestia {

class BaseResponse {
  public:
    enum class State { OK, ERROR };
    using Ptr = std::unique_ptr<BaseResponse>;

    BaseResponse(const BaseRequest& request) : m_request_id(request.get_id()) {}

    virtual ~BaseResponse() = default;

    virtual bool ok() const { return m_state == State::OK; }

    const BaseRequestError& get_base_error() const { return m_base_error; }

    void on_base_error(const BaseRequestError& error)
    {
        m_base_error = error;
        m_state      = State::ERROR;
    }

  protected:
    std::string m_request_id;
    State m_state{State::OK};
    BaseRequestError m_base_error;
};

template<typename ErrorCode>
class Response : public BaseResponse {
  public:
    Response(const BaseRequest& request) : BaseResponse(request) {}

    virtual ~Response() = default;

    const RequestError<ErrorCode>& get_error() const { return m_error; }

    void on_error(const RequestError<ErrorCode>& error)
    {
        m_error = error;
        on_base_error(error);
    }

  protected:
    RequestError<ErrorCode> m_error;
};

}  // namespace hestia