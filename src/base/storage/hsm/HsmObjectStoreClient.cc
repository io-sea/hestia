#include "HsmObjectStoreClient.h"

#include "ErrorUtils.h"
#include "Logger.h"

#include <algorithm>
#include <stdexcept>

#define CATCH_FLOW()                                                           \
    catch (const std::exception& e)                                            \
    {                                                                          \
        on_exception(request, response.get(), e.what());                       \
        completion_func(std::move(response));                                  \
        return;                                                                \
    }                                                                          \
    catch (...)                                                                \
    {                                                                          \
        on_exception(request, response.get());                                 \
        completion_func(std::move(response));                                  \
        return;                                                                \
    }

namespace hestia {

HsmObjectStoreClient::~HsmObjectStoreClient() {}

void HsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    Stream* stream) const noexcept
{
    auto response = HsmObjectStoreResponse::create(request, m_id);
    switch (request.method()) {
        case HsmObjectStoreRequestMethod::GET:
            try {
                get(request, stream, completion_func, progress_func);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::PUT:
            try {
                put(request, stream, completion_func, progress_func);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::MOVE:
            try {
                move(request, completion_func, progress_func);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::COPY:
            try {
                copy(request, completion_func, progress_func);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::REMOVE:
            try {
                remove(request, completion_func, progress_func);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::REMOVE_ALL:
            LOG_ERROR("Not implemented yet: ");
            break;
        case HsmObjectStoreRequestMethod::EXISTS:
            LOG_ERROR("Not implemented yet: ");
            break;
        default:
            const std::string msg =
                "Method: " + request.method_as_string() + " not supported";
            const HsmObjectStoreError error(
                HsmObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
    }
    completion_func(std::move(response));
}

void HsmObjectStoreClient::make_request(
    const ObjectStoreRequest& request,
    ObjectStoreClient::completionFunc completion_func,
    ObjectStoreClient::progressFunc progress_func,
    Stream* stream) const noexcept
{
    auto response = ObjectStoreResponse::create(request, m_id);
    if (!HsmObjectStoreRequest::is_hsm_supported_method(request.method())) {
        const std::string msg =
            "Requested unsupported type for base object operation in HSM object store client.";
        response->on_error(
            {ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg});
        completion_func(std::move(response));
    }

    HsmObjectStoreRequest hsm_request(request);

    auto hsm_store_completion_func =
        [completion_func,
         hsm_request](HsmObjectStoreResponse::Ptr hsm_response) {
            completion_func(HsmObjectStoreResponse::to_base_response(
                hsm_request, hsm_response.get()));
        };

    auto hsm_store_progress_func =
        [progress_func, hsm_request](HsmObjectStoreResponse::Ptr hsm_response) {
            progress_func(HsmObjectStoreResponse::to_base_response(
                hsm_request, hsm_response.get()));
        };

    make_request(
        hsm_request, hsm_store_completion_func, hsm_store_progress_func,
        stream);
}

void HsmObjectStoreClient::set_tier_ids(
    const std::vector<std::string>& tier_ids)
{
    m_tier_ids = tier_ids;
}

std::size_t HsmObjectStoreClient::get_tier_index(const std::string& id) const
{
    auto it = std::find(m_tier_ids.begin(), m_tier_ids.end(), id);
    if (it == m_tier_ids.end()) {
        throw std::runtime_error(
            "Requested tier id " + id + "not found in object store cache.");
    }
    return std::distance(m_tier_ids.begin(), it);
}

bool HsmObjectStoreClient::exists(const StorageObject&) const
{
    throw std::runtime_error("Exists operation not supported for hsm objects");
}

void HsmObjectStoreClient::list(
    const KeyValuePair&, std::vector<StorageObject>&) const
{
    throw std::runtime_error("List operation not supported for hsm objects");
}

void HsmObjectStoreClient::get(
    const ObjectStoreRequest&,
    ObjectStoreClient::completionFunc,
    Stream*,
    Stream::progressFunc) const
{
    throw std::runtime_error(SOURCE_LOC() + " | Not implemented.");
}

void HsmObjectStoreClient::put(
    const ObjectStoreRequest&,
    ObjectStoreClient::completionFunc,
    Stream*,
    Stream::progressFunc) const
{
    throw std::runtime_error(SOURCE_LOC() + " | Not implemented.");
}

void HsmObjectStoreClient::remove(const StorageObject&) const
{
    throw std::runtime_error(SOURCE_LOC() + " | Not implemented.");
}

void HsmObjectStoreClient::on_exception(
    const HsmObjectStoreRequest& request,
    HsmObjectStoreResponse* response,
    const std::string& message) const
{
    if (!message.empty()) {
        const std::string msg = "Exception in " + request.method_as_string()
                                + " method: " + message;
        const HsmObjectStoreError error(
            HsmObjectStoreErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
    else {
        const std::string msg =
            "Uknown Exception in " + request.method_as_string() + " method";
        const HsmObjectStoreError error(
            HsmObjectStoreErrorCode::UNKNOWN_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
}
}  // namespace hestia