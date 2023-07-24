#pragma once

#include <string>

namespace hestia {
class BaseRequest {
  public:
    BaseRequest(const std::string& url = {}, const std::string& id = {}) :
        m_url(url), m_id(id)
    {
    }

    virtual ~BaseRequest() = default;

    const std::string& get_id() const { return m_id; }

    const std::string& get_url() const { return m_url; }

  private:
    std::string m_url;
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