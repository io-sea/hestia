#pragma once

#include <string>

class CopyToolError {
  public:
    enum class Code {
        NO_ERROR,
        ERROR_NO_CONNECTION,
        ERROR_OBJECT_STORE,
        MAX_ERROR
    };

    CopyToolError() = default;

    CopyToolError(Code code, const std::string& message) :
        m_code(code), mMessage(message)
    {
    }

  private:
    Code m_code{Code::NO_ERROR};
    std::string m_message;
};