#include "CrudService.h"

#include "CrudClient.h"
#include "EventFeed.h"
#include "IdGenerator.h"
#include "TimeProvider.h"

#include "ErrorUtils.h"
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
    EventFeed* event_feed,
    std::unique_ptr<IdGenerator> id_generator,
    std::unique_ptr<TimeProvider> time_provider) :
    Service<CrudRequest, CrudResponse, CrudErrorCode>(config),
    m_event_feed(event_feed),
    m_client(std::move(client)),
    m_id_generator(std::move(id_generator)),
    m_time_provider(std::move(time_provider))
{
}

CrudService::~CrudService() {}

[[nodiscard]] CrudResponse::Ptr CrudService::make_request(
    const CrudRequest& request, const std::string&) const noexcept
{
    LOG_INFO(
        "Starting Subject: " << get_type()
                             << ", Method: " << request.method_as_string());

    auto response = std::make_unique<CrudResponse>(
        request, get_type(), request.get_output_format());

    bool record_modified_attrs{false};
    if (m_event_feed != nullptr) {
        record_modified_attrs =
            m_event_feed->will_handle(get_type(), request.method());
    }

    switch (request.method()) {
        case CrudMethod::CREATE:
            try {
                create_crud(request, *response, record_modified_attrs);
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::READ:
            try {
                read(request, *response);
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::UPDATE:
            try {
                update(request, *response, record_modified_attrs);
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::REMOVE:
            try {
                remove(request, *response);
            }
            HESTIA_CRUD_SERVICE_CATCH_FLOW();
            break;
        case CrudMethod::IDENTIFY:
            try {
                identify(request, *response);
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

    LOG_INFO(
        "Finished Subject: " << get_type()
                             << ", Method: " << request.method_as_string());

    try {
        if (m_event_feed != nullptr && request.should_update_event_feed()) {
            m_event_feed->on_event(
                CrudEvent(get_type(), request.method(), request, *response));
        }
    }
    HESTIA_CRUD_SERVICE_CATCH_FLOW();

    return response;
}

void CrudService::create_crud(
    const CrudRequest& request,
    CrudResponse& response,
    bool record_modified_attrs) const
{
    m_client->create(request, response, record_modified_attrs);
}

void CrudService::read(const CrudRequest& request, CrudResponse& response) const
{
    return m_client->read(request, response);
}

void CrudService::set_default_name(const std::string& name)
{
    m_config.m_default_name = name;
}

void CrudService::update(
    const CrudRequest& request,
    CrudResponse& response,
    bool record_modified_attrs) const
{
    m_client->update(request, response, record_modified_attrs);
}

void CrudService::remove(
    const CrudRequest& request, CrudResponse& response) const
{
    m_client->remove(request, response);
}

void CrudService::identify(
    const CrudRequest& request, CrudResponse& response) const
{
    m_client->identify(request, response);
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

const std::string& CrudService::get_default_name() const
{
    return m_config.m_default_name;
}

void CrudService::register_parent_service(
    const std::string& type, CrudService* service)
{
    m_client->register_parent_service(type, service);
}

void CrudService::register_child_service(
    const std::string& type, CrudService* service)
{
    m_client->register_child_service(type, service);
}

void CrudService::on_exception(
    const CrudRequest& request,
    CrudResponse* response,
    const std::string& message) const
{
    if (!message.empty()) {
        const std::string msg = SOURCE_LOC() + " | Exception in "
                                + request.method_as_string() + " method.\n"
                                + message;
        const CrudRequestError error(CrudErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error << " " << msg);
        response->on_error(error);
    }
    else {
        const std::string msg = SOURCE_LOC() + " | Uknown Exception in "
                                + request.method_as_string() + " method.";
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
    const std::string msg = SOURCE_LOC() + " | Error in "
                            + request.method_as_string() + " method: ";
    LOG_ERROR(msg << error);
    response->on_error(error);
}


}  // namespace hestia