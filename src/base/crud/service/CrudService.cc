#include "CrudService.h"

#include "CrudClient.h"
#include "IdGenerator.h"
#include "TimeProvider.h"

#include "Logger.h"

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

CrudService::CrudService(
    const ServiceConfig& config,
    std::unique_ptr<CrudClient> client,
    std::unique_ptr<IdGenerator> id_generator,
    std::unique_ptr<TimeProvider> time_provider) :
    Service<CrudRequest, CrudResponse, CrudErrorCode>(config),
    m_client(std::move(client)),
    m_id_generator(std::move(id_generator)),
    m_time_provider(std::move(time_provider))
{
}

CrudService::~CrudService() {}

[[nodiscard]] CrudResponse::Ptr CrudService::make_request(
    const CrudRequest& request, const std::string&) const noexcept
{
    LOG_INFO("Request " << request.method_as_string());

    auto response = std::make_unique<CrudResponse>(request);

    switch (request.method()) {
        case CrudMethod::CREATE:
            try {
                create(request, *response);
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::READ:
            try {
                read(request.get_query(), *response);
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::UPDATE:
            try {
                update(request, *response);
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::REMOVE:
            try {
                remove(request.get_ids());
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::IDENTIFY:
            try {
                identify(request.get_ids(), *response);
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::LOCK:
            try {
                lock(request.get_id(), request.lock_type());
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::UNLOCK:
            try {
                lock(request.get_id(), request.lock_type());
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::LOCKED:
            try {
                const auto locked =
                    is_locked(request.get_id(), request.lock_type());
                response->set_locked(locked);
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

void CrudService::create(
    const CrudRequest& request, CrudResponse& response) const
{
    m_client->create(request, response);
}

void CrudService::read(const CrudQuery& query, CrudResponse& response) const
{
    return m_client->read(query, response);
}

void CrudService::update(
    const CrudRequest& request, CrudResponse& response) const
{
    m_client->update(request, response);
}

void CrudService::remove(const VecCrudIdentifier& ids) const
{
    m_client->remove(ids);
}

void CrudService::identify(
    const VecCrudIdentifier& ids, CrudResponse& response) const
{
    m_client->identify(ids, response);
}

void CrudService::lock(const CrudIdentifier& id, CrudLockType lock_type) const
{
    m_client->lock(id, lock_type);
}

void CrudService::unlock(const CrudIdentifier& id, CrudLockType lock_type) const
{
    m_client->unlock(id, lock_type);
}

bool CrudService::is_locked(
    const CrudIdentifier& id, CrudLockType lock_type) const
{
    return m_client->is_locked(id, lock_type);
}

std::string CrudService::get_type() const
{
    return m_client->get_type();
}

void CrudService::on_exception(
    const CrudRequest& request,
    CrudResponse* response,
    const std::string& message) const
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

void CrudService::on_exception(
    const CrudRequest& request,
    CrudResponse* response,
    const RequestError<CrudErrorCode>& error) const
{
    const std::string msg =
        "Error in " + request.method_as_string() + " method: ";
    LOG_ERROR(msg << error);
    response->on_error(error);
}


}  // namespace hestia