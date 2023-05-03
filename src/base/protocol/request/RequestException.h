#pragma once

#include <exception>

namespace hestia {

template<typename ERROR>
class RequestException : public std::exception {
  public:
    RequestException(const ERROR& error) : m_error(error) {}

    const ERROR& get_error() const { return m_error; }

  private:
    ERROR m_error;
};
}  // namespace hestia