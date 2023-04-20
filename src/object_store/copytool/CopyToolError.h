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
        mCode(code), mMessage(message)
    {
    }

  private:
    Code mCode{Code::NO_ERROR};
    std::string mMessage;
};