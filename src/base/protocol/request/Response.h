#pragma once

#include "Request.h"
#include "RequestError.h"

#include <memory>
#include <vector>

namespace hestia {

class BaseResponse {
  public:
    enum class State { OK, ERROR };
    using Ptr = std::unique_ptr<BaseResponse>;

    BaseResponse(const BaseRequest& request) : m_request_id(request.get_id()) {}

    virtual ~BaseResponse() = default;

    virtual bool ok() const { return m_state == State::OK; }

    const BaseRequestError& get_base_error() const { return m_base_error; }

    void on_base_error(const BaseRequestError& error)
    {
        m_base_error = error;
        m_state      = State::ERROR;
    }

  protected:
    std::string m_request_id;
    State m_state{State::OK};
    BaseRequestError m_base_error;
};

template<typename ErrorCode>
class Response : public BaseResponse {
  public:
    Response(const BaseRequest& request) : BaseResponse(request) {}

    virtual ~Response() = default;

    const RequestError<ErrorCode>& get_error() const { return m_error; }

    void on_error(const RequestError<ErrorCode>& error)
    {
        m_error = error;
        on_base_error(error);
    }

  protected:
    RequestError<ErrorCode> m_error;
};

template<typename ItemType, typename ErrorCode>
class CrudResponse : public Response<ErrorCode> {
  public:
    CrudResponse(const CrudRequest<ItemType>& request) :
        Response<ErrorCode>(request), m_item(request.item())
    {
    }

    CrudResponse(const BaseRequest& request) : Response<ErrorCode>(request) {}

    virtual ~CrudResponse() = default;

    const std::string& query_result() const { return m_query_result; };

    const std::vector<ItemType>& items() const { return m_items; };

    std::vector<ItemType>& items() { return m_items; }

    const ItemType& item() const { return m_item; }

    ItemType& item() { return m_item; }

    const std::vector<std::string>& ids() const { return m_ids; };

    std::vector<std::string>& ids() { return m_ids; };

    bool found() const { return m_found; }

    void set_found(bool found) { m_found = found; }

  private:
    std::string m_query_result;
    std::vector<ItemType> m_items;
    std::vector<std::string> m_ids;
    ItemType m_item;
    bool m_found{false};

  protected:
    RequestError<ErrorCode> m_error;
};

}  // namespace hestia