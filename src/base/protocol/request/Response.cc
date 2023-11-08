#include "Response.h"

namespace hestia {

BaseResponse::BaseResponse(const BaseRequest& request) :
    m_request_id(request.get_id())
{
}

bool BaseResponse::ok() const
{
    return m_state == State::OK;
}

const BaseRequestError& BaseResponse::get_base_error() const
{
    return m_base_error;
}

void BaseResponse::on_base_error(const BaseRequestError& error)
{
    m_base_error = error;
    m_state      = State::ERROR;
}
}  // namespace hestia