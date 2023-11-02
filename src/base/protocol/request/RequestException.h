#pragma once

#include <exception>

namespace hestia {

template<typename ERROR>
class RequestException : public std::exception {
  public:
    RequestException(const ERROR& error) : m_error(error)
    {
        m_msg = m_error.to_string();
    }

    const ERROR& get_error() const { return m_error; }

    const char* what() const noexcept override { return m_msg.c_str(); }

  private:
    std::string m_msg;
    ERROR m_error;
};
}  // namespace hestia