#pragma once

#include <ostream>
#include <string>

namespace hestia {
class HttpError {
  public:
    enum class Code {
        _100_CONTINUE,
        _200_OK,
        _201_CREATED,
        _204_NO_CONTENT,
        _400_BAD_REQUEST,
        _403_FORBIDDEN,
        _404_NOT_FOUND,
        _409_CONFLICT,
        _411_LENGTH_REQURED,
        _500_INTERNAL_SERVER_ERROR,
        CUSTOM
    };

    HttpError(Code code = Code::_200_OK, const std::string& message = {});

    bool equals(const HttpError& other) const;

    using CodeAndId = std::pair<unsigned short, std::string>;
    CodeAndId get_code_and_id() const;

    const std::string& get_message() const;

    virtual std::string to_string() const;

    bool operator==(const HttpError& other) const { return equals(other); }

    bool operator!=(const HttpError& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, HttpError const& value)
    {
        os << value.to_string();
        return os;
    }

  protected:
    Code m_code{Code::_200_OK};
    std::string m_message;
};
}  // namespace hestia