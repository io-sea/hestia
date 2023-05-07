#pragma once

#include <string>

namespace hestia {
template<typename ErrorCode>
class RequestError {
  public:
    RequestError() = default;

    RequestError(ErrorCode code, const std::string& message) :
        m_message(message), m_code(code)
    {
    }

    virtual ~RequestError() = default;

    ErrorCode code() const { return m_code; }

    virtual std::string code_as_string() const { return {}; }

    const std::string& message() const { return m_message; }

    int number() const { return static_cast<int>(m_code); }

    virtual std::string to_string() const
    {
        return "# " + std::to_string(number()) + " | " + code_as_string()
               + " | " + m_message;
    }

    friend std::ostream& operator<<(
        std::ostream& stream, const RequestError& error)
    {
        stream << error.to_string();
        return stream;
    }

  protected:
    std::string m_message;
    ErrorCode m_code;
};
}  // namespace hestia