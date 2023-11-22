#include "HttpStatus.h"

#include <unordered_map>

namespace hestia {
static std::unordered_map<HttpStatus::Code, HttpStatus::CodeAndId> errors = {
    {HttpStatus::Code::_100_CONTINUE, {100, "Continue"}},
    {HttpStatus::Code::_200_OK, {200, "OK"}},
    {HttpStatus::Code::_201_CREATED, {201, "Created"}},
    {HttpStatus::Code::_204_NO_CONTENT, {201, "No Content"}},
    {HttpStatus::Code::_400_BAD_REQUEST, {400, "Bad Request"}},
    {HttpStatus::Code::_401_UNAUTHORIZED, {401, "Unauthorized"}},
    {HttpStatus::Code::_403_FORBIDDEN, {403, "Forbidden"}},
    {HttpStatus::Code::_404_NOT_FOUND, {404, "Not Found"}},
    {HttpStatus::Code::_409_CONFLICT, {409, "Conflict"}},
    {HttpStatus::Code::_411_LENGTH_REQURED, {411, "Length Required"}},
    {HttpStatus::Code::_500_INTERNAL_SERVER_ERROR,
     {500, "Internal Server Error"}},
    {HttpStatus::Code::CUSTOM, {0, "Custom"}},
};

HttpStatus::HttpStatus(Code code, const std::string& message) :
    m_code(code), m_message(message)
{
}

bool HttpStatus::equals(const HttpStatus& other) const
{
    return m_code == other.m_code && m_message == other.m_message;
}

HttpStatus::Code HttpStatus::get_code_from_numeric(unsigned code)
{
    for (const auto& error : errors) {
        if (error.second.first == code) {
            return error.first;
        }
    }
    return HttpStatus::Code::_500_INTERNAL_SERVER_ERROR;
}

HttpStatus::CodeAndId HttpStatus::get_code_and_id() const
{
    unsigned short code    = 500;
    std::string identifier = "Internal Server Error";
    if (const auto& iter = errors.find(m_code); iter != errors.end()) {
        code       = iter->second.first;
        identifier = iter->second.second;
    }
    return {code, identifier};
}

const std::string& HttpStatus::get_message() const
{
    return m_message;
}

std::string HttpStatus::to_string() const
{
    const auto& [code, identifier] = get_code_and_id();
    return "Code: " + std::to_string(code) + " Id: " + identifier + " | "
           + m_message;
}
}  // namespace hestia