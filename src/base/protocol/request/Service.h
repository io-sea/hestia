#pragma once

#include "Request.h"
#include "RequestError.h"
#include "RequestException.h"
#include "Response.h"

#include "Logger.h"

#include <memory>
#include <string>

namespace hestia {

template<typename RequestT, typename ResponseT, typename ErrorCodeT>
class Service {
  public:
    Service() = default;

    virtual ~Service() = default;

    [[nodiscard]] virtual std::unique_ptr<ResponseT> make_request(
        const RequestT& request) const noexcept = 0;

  private:
    virtual void on_exception(
        const RequestT& request,
        ResponseT* response,
        const std::string& message = {}) const = 0;

    virtual void on_exception(
        const RequestT& request,
        ResponseT* response,
        const RequestError<ErrorCodeT>& error) const = 0;
};

#define HESTIA_CRUD_SERVICE_CATCH_FLOW()                                       \
    catch (const RequestException<RequestError<CrudErrorCode>>& e)             \
    {                                                                          \
        on_exception(request, response.get(), e.get_error());                  \
        return response;                                                       \
    }                                                                          \
    catch (const std::exception& e)                                            \
    {                                                                          \
        on_exception(request, response.get(), e.what());                       \
        return response;                                                       \
    }                                                                          \
    catch (...)                                                                \
    {                                                                          \
        on_exception(request, response.get());                                 \
        return response;                                                       \
    }

template<typename ItemT>
class CrudService :
    public Service<
        CrudRequest<ItemT>,
        CrudResponse<ItemT, CrudErrorCode>,
        CrudErrorCode> {
  public:
    CrudService() :
        Service<
            CrudRequest<ItemT>,
            CrudResponse<ItemT, CrudErrorCode>,
            CrudErrorCode>(){};

    virtual ~CrudService() = default;

    [[nodiscard]] std::unique_ptr<CrudResponse<ItemT, CrudErrorCode>>
    make_request(const CrudRequest<ItemT>& request) const noexcept override
    {
        auto response =
            std::make_unique<CrudResponse<ItemT, CrudErrorCode>>(request);

        switch (request.method()) {
            case CrudMethod::GET:
                try {
                    get(response->item());
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            case CrudMethod::PUT:
                try {
                    put(request.item());
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            case CrudMethod::EXISTS:
                try {
                    response->set_found(exists(request.item()));
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            case CrudMethod::LIST:
                try {
                    list(response->items());
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            case CrudMethod::REMOVE:
                try {
                    remove(request.item());
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            default:
                const std::string msg =
                    "Method: " + request.method_as_string() + " not supported";
                const CrudRequestError error(
                    CrudErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
                LOG_ERROR("Error: " << error);
                response->on_error(error);
                return response;
        }

        return response;
    }

  protected:
    virtual void get(ItemT& item) const = 0;

    virtual void put(const ItemT& item) const = 0;

    virtual bool exists(const ItemT& item) const = 0;

    virtual void remove(const ItemT& item) const = 0;

    virtual void list(std::vector<ItemT>& item) const = 0;

  private:
    void on_exception(
        const CrudRequest<ItemT>& request,
        CrudResponse<ItemT, CrudErrorCode>* response,
        const std::string& message = {}) const override
    {
        if (!message.empty()) {
            const std::string msg = "Exception in " + request.method_as_string()
                                    + " method: " + message;
            const CrudRequestError error(CrudErrorCode::STL_EXCEPTION, msg);
            LOG_ERROR("Error: " << error << " " << msg);
            response->on_error(error);
        }
        else {
            const std::string msg =
                "Uknown Exception in " + request.method_as_string() + " method";
            const CrudRequestError error(CrudErrorCode::UNKNOWN_EXCEPTION, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
        }
    }

    void on_exception(
        const CrudRequest<ItemT>& request,
        CrudResponse<ItemT, CrudErrorCode>* response,
        const RequestError<CrudErrorCode>& error) const override
    {
        const std::string msg =
            "Error in " + request.method_as_string() + " method: ";
        LOG_ERROR(msg << error);
        response->on_error(error);
    }
};

}  // namespace hestia