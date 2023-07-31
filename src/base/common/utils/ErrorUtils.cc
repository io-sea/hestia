#include "ErrorUtils.h"

#include <filesystem>

namespace hestia {
OpStatus::OpStatus(
    Status status, int error_code, const std::string& error_message) :
    m_status(status), m_error_code(error_code), m_error_message(error_message)
{
}

std::string OpStatus::get_location_prefix(
    const std::string& file_name,
    const std::string& function_name,
    int line_number)
{
    return std::filesystem::path(file_name).stem().string()
           + "::" + function_name + "::" + std::to_string(line_number);
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