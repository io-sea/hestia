#include "HsmObjectStoreClient.h"

#include "ErrorUtils.h"
#include "Logger.h"

#include <algorithm>
#include <stdexcept>

#define CATCH_FLOW()                                                           \
    catch (const std::exception& e)                                            \
    {                                                                          \
        on_exception(ctx.m_request, response.get(), e.what());                 \
        ctx.finish(std::move(response));                                       \
        return;                                                                \
    }                                                                          \
    catch (...)                                                                \
    {                                                                          \
        on_exception(ctx.m_request, response.get());                           \
        ctx.finish(std::move(response));                                       \
        return;                                                                \
    }

namespace hestia {

HsmObjectStoreClient::~HsmObjectStoreClient() {}

void HsmObjectStoreClient::make_request(
    HsmObjectStoreContext& ctx) const noexcept
{
    auto response = HsmObjectStoreResponse::create(ctx.m_request, m_id);
    switch (ctx.m_request.method()) {
        case HsmObjectStoreRequestMethod::GET:
            try {
                get(ctx);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::PUT:
            try {
                put(ctx);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::MOVE:
            try {
                move(ctx);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::COPY:
            try {
                copy(ctx);
            }
            CATCH_FLOW();
            return;
        case HsmObjectStoreRequestMethod::REMOVE:
            try {
                remove(ctx);
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
                "Method: " + ctx.m_request.method_as_string()
                + " not supported";
            const HsmObjectStoreError error(
                HsmObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
    }
    ctx.finish(std::move(response));
}

void HsmObjectStoreClient::make_request(ObjectStoreContext& ctx) const noexcept
{
    auto response = ObjectStoreResponse::create(ctx.m_request, m_id);
    if (!HsmObjectStoreRequest::is_hsm_supported_method(
            ctx.m_request.method())) {
        const std::string msg =
            "Requested unsupported type for base object operation in HSM object store client.";
        response->on_error(
            {ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg});
        ctx.finish(std::move(response));
    }

    HsmObjectStoreRequest hsm_request(ctx.m_request);

    auto hsm_store_completion_func =
        [completion_func = ctx.get_completion_func(),
         hsm_request](HsmObjectStoreResponse::Ptr hsm_response) {
            completion_func(HsmObjectStoreResponse::to_base_response(
                hsm_request, hsm_response.get()));
        };

    auto hsm_store_progress_func =
        [progress_func = ctx.m_progress_func,
         hsm_request](HsmObjectStoreResponse::Ptr hsm_response) {
            progress_func(HsmObjectStoreResponse::to_base_response(
                hsm_request, hsm_response.get()));
        };

    HsmObjectStoreContext hsm_ctx(
        hsm_request, hsm_store_completion_func, hsm_store_progress_func,
        ctx.m_stream);

    make_request(hsm_ctx);
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

void HsmObjectStoreClient::get(ObjectStoreContext&) const
{
    throw std::runtime_error(SOURCE_LOC() + " | Not implemented.");
}

void HsmObjectStoreClient::put(ObjectStoreContext&) const
{
    throw std::runtime_error(SOURCE_LOC() + " | Not implemented.");
}

void HsmObjectStoreClient::remove(const StorageObject&) const
{
    throw std::runtime_error(SOURCE_LOC() + " | Not implemented.");
}

void HsmObjectStoreClient::on_success(const HsmObjectStoreContext& ctx) const
{
    ctx.finish(HsmObjectStoreResponse::create(ctx.m_request, m_id));
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