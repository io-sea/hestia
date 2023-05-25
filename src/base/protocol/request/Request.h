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

enum class CrudMethod { PUT, GET, EXISTS, REMOVE, LIST };

template<typename ItemT>
class CrudRequest : public BaseRequest, public MethodRequest<CrudMethod> {
  public:
    CrudRequest(const ItemT& item, CrudMethod method) :
        BaseRequest(), MethodRequest<CrudMethod>(method), m_item(item)
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