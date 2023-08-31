#include "HttpError.h"

#include <unordered_map>

namespace hestia {
static std::unordered_map<HttpError::Code, HttpError::CodeAndId> errors = {
    {HttpError::Code::_100_CONTINUE, {100, "Continue"}},
    {HttpError::Code::_200_OK, {200, "OK"}},
    {HttpError::Code::_201_CREATED, {201, "Created"}},
    {HttpError::Code::_204_NO_CONTENT, {201, "No Content"}},
    {HttpError::Code::_400_BAD_REQUEST, {400, "Bad Request"}},
    {HttpError::Code::_403_FORBIDDEN, {403, "Forbidden"}},
    {HttpError::Code::_404_NOT_FOUND, {404, "Not Found"}},
    {HttpError::Code::_409_CONFLICT, {409, "Conflict"}},
    {HttpError::Code::_411_LENGTH_REQURED, {411, "Length Required"}},
    {HttpError::Code::_500_INTERNAL_SERVER_ERROR,
     {500, "Internal Server Error"}},
    {HttpError::Code::CUSTOM, {0, "Custom"}},
};

HttpError::HttpError(Code code, const std::string& message) :
    m_code(code), m_message(message)
{
}

bool HttpError::equals(const HttpError& other) const
{
    return m_code == other.m_code && m_message == other.m_message;
}

HttpError::CodeAndId HttpError::get_code_and_id() const
{
    unsigned short code    = 500;
    std::string identifier = "Internal Server Error";
    if (const auto& iter = errors.find(m_code); iter != errors.end()) {
        code       = iter->second.first;
        identifier = iter->second.second;
    }
    return {code, identifier};
}

const std::string& HttpError::get_message() const
{
    return m_message;
}

std::string HttpError::to_string() const
{
    const auto& [code, identifier] = get_code_and_id();
    return "Code: " + std::to_string(code) + " Id: " + identifier + " | "
           + m_message;
}
}  // namespace hestia