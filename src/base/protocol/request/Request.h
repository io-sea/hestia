#pragma once

#include <string>

namespace hestia {
class BaseRequest {
  public:
    BaseRequest() {}

    virtual ~BaseRequest() = default;

    const std::string& get_id() const { return m_id; }

  private:
    std::string m_id;
};

template<typename METHOD>
class MethodRequest {
  public:
    MethodRequest(METHOD method) : m_method(method) {}

    virtual ~MethodRequest() = default;

    METHOD method() const { return m_method; }

    void reset_method(METHOD method) { m_method = method; }

    virtual std::string method_as_string() const = 0;

  protected:
    METHOD m_method;
};
}  // namespace hestia