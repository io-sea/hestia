#pragma once

#include <string>

namespace hestia {

class BaseRequestError {
  public:
    BaseRequestError(const std::string& message = {}) : m_message(message) {}

    virtual ~BaseRequestError() = default;

    const std::string& message() const { return m_message; }

    virtual std::string to_string() const { return m_message; }

  protected:
    std::string m_message;
};


template<typename ErrorCode>
class RequestError : public BaseRequestError {
  public:
    RequestError() = default;

    RequestError(ErrorCode code, const std::string& message) :
        BaseRequestError(message), m_code(code)
    {
    }

    virtual ~RequestError() = default;

    ErrorCode code() const { return m_code; }

    virtual std::string code_as_string() const { return {}; }

    int number() const { return static_cast<int>(m_code); }

    std::string to_string() const override
    {
        return "Code: " + std::to_string(number()) + " | " + code_as_string()
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

enum class CrudErrorCode {
    NO_ERROR,
    ERROR,
    STL_EXCEPTION,
    UNKNOWN_EXCEPTION,
    UNSUPPORTED_REQUEST_METHOD,
    ITEM_NOT_FOUND,
    CANT_OVERRIDE_EXISTING,
    NOT_AUTHENTICATED,
    MAX_ERROR
};

class CrudRequestError : public RequestError<CrudErrorCode> {
  public:
    CrudRequestError() = default;

    CrudRequestError(CrudErrorCode code, const std::string& message) :
        RequestError(code, message)
    {
    }
    virtual ~CrudRequestError() = default;
};


}  // namespace hestia