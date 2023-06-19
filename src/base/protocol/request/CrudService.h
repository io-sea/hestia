#pragma once

#include "CrudClient.h"
#include "Service.h"

#include <memory>
#include <string>

namespace hestia {

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
    CrudService(std::unique_ptr<CrudClient<ItemT>> client) :
        Service<
            CrudRequest<ItemT>,
            CrudResponse<ItemT, CrudErrorCode>,
            CrudErrorCode>(),
        m_client(std::move(client)){};

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
            case CrudMethod::MULTI_GET:
                try {
                    multi_get(request.query(), response->items());
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            case CrudMethod::PUT:
                try {
                    put(request.item(), request.should_generate_id());
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            case CrudMethod::EXISTS:
                try {
                    response->set_found(exists(request.item().id()));
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            case CrudMethod::LIST:
                try {
                    list(request.query(), response->ids());
                }
                HESTIA_CRUD_SERVICE_CATCH_FLOW();
                break;
            case CrudMethod::REMOVE:
                try {
                    remove(request.item().id());
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
    virtual void get(ItemT& item) const { m_client->get(item); }

    virtual bool exists(const std::string& id) const
    {
        return m_client->exists(id);
    }

    virtual void list(
        const Metadata& query, std::vector<std::string>& ids) const
    {
        m_client->list(query, ids);
    }

    virtual void multi_get(
        const Metadata& query, std::vector<ItemT>& items) const
    {
        m_client->multi_get(query, items);
    }

    virtual void put(const ItemT& item, bool generate_id = false) const
    {
        m_client->put(item, generate_id);
    }

    virtual void remove(const std::string& id) const { m_client->remove(id); }

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

    std::unique_ptr<CrudClient<ItemT>> m_client;
};

}  // namespace hestia
