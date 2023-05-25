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

enum class CrudMethod { PUT, GET, MULTI_GET, EXISTS, REMOVE, LIST };

template<typename ItemT>
class CrudRequest : public BaseRequest, public MethodRequest<CrudMethod> {
  public:
    CrudRequest(const ItemT& item, CrudMethod method) :
        BaseRequest(), MethodRequest<CrudMethod>(method), m_item(item)
    {
    }

    CrudRequest(CrudMethod method) :
        BaseRequest(), MethodRequest<CrudMethod>(method)
    {
    }

    virtual ~CrudRequest() = default;

    std::string method_as_string() const override
    {
        switch (m_method) {
            case CrudMethod::PUT:
                return "PUT";
            case CrudMethod::GET:
                return "GET";
            case CrudMethod::MULTI_GET:
                return "MULTI_GET";
            case CrudMethod::EXISTS:
                return "EXISTS";
            case CrudMethod::REMOVE:
                return "REMOVE";
            case CrudMethod::LIST:
                return "LIST";
            default:
                return "UNKNOWN";
        }
    };

    const ItemT& item() const { return m_item; };

  private:
    ItemT m_item;
    std::string m_query;
};

}  // namespace hestia