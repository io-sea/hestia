#include "ErrorUtils.h"

namespace hestia {
OpStatus::OpStatus(
    Status status, int error_code, const std::string& error_message) :
    m_status(status), m_error_code(error_code), m_error_message(error_message)
{
}

std::string OpStatus::code() const
{
    return std::to_string(m_error_code);
}

const std::string& OpStatus::message() const
{
    return m_error_message;
}

bool OpStatus::ok() const
{
    return m_status == Status::OK;
}

std::string OpStatus::str() const
{
    return ok() ? "OK" : "ERROR: " + code() + " | " + message();
}
}  // namespace hestia