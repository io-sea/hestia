#include "HestiaClient.h"

#include "HestiaConfig.h"
#include "hestia.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "HsmServicesFactory.h"
#include "UserService.h"

#include "ErrorUtils.h"
#include "FileHsmObjectStoreClient.h"
#include "HsmObjectStoreClient.h"
#include "HttpClient.h"
#include "JsonUtils.h"

#include "Logger.h"

#include <iostream>
#include <stdexcept>

namespace hestia {

namespace rc {
OpStatus error(const RequestError<HsmActionErrorCode>& error)
{
    switch (error.code()) {
        case HsmActionErrorCode::NO_ERROR:
            return {};
        case HsmActionErrorCode::ERROR:
        case HsmActionErrorCode::CRUD_ERROR:
        case HsmActionErrorCode::OBJECT_STORE_ERROR:
        case HsmActionErrorCode::STL_EXCEPTION:
        case HsmActionErrorCode::UNKNOWN_EXCEPTION:
        case HsmActionErrorCode::UNSUPPORTED_REQUEST_METHOD:
        case HsmActionErrorCode::ITEM_NOT_FOUND:
        case HsmActionErrorCode::BAD_PUT_OVERWRITE_COMBINATION:
        case HsmActionErrorCode::MAX_ERROR:
        default:
            return {
                OpStatus::Status::ERROR, hestia_error_t::HESTIA_ERROR_UNKNOWN,
                error.message()};
    }
}

OpStatus error(const RequestError<CrudErrorCode>& error)
{
    switch (error.code()) {
        case CrudErrorCode::NO_ERROR:
            return {};
        case CrudErrorCode::ERROR:
        case CrudErrorCode::STL_EXCEPTION:
        case CrudErrorCode::NOT_AUTHENTICATED:
        case CrudErrorCode::UNKNOWN_EXCEPTION:
        case CrudErrorCode::UNSUPPORTED_REQUEST_METHOD:
        case CrudErrorCode::ITEM_NOT_FOUND:
        case CrudErrorCode::CANT_OVERRIDE_EXISTING:
        case CrudErrorCode::MAX_ERROR:
        default:
            return {
                OpStatus::Status::ERROR, hestia_error_t::HESTIA_ERROR_UNKNOWN,
                error.message()};
    }
}

OpStatus bad_stream()
{
    return {
        OpStatus::Status::ERROR, hestia_error_t::HESTIA_ERROR_BAD_STREAM,
        "Stream failed"};
}

}  // namespace rc

#define ERROR_CHECK(response, method_str)                                      \
    if (!response->ok()) {                                                     \
        set_last_error(                                                        \
            "Error in " + std::string(method_str) + ": "                       \
            + response->get_error().to_string());                              \
        return rc::error(response->get_error());                               \
    }

HestiaClient::HestiaClient() : HestiaApplication() {}

HestiaClient::~HestiaClient() {}

OpStatus HestiaClient::initialize(
    const std::string& config_path,
    const std::string& user_token,
    const Dictionary& extra_config,
    const std::string& server_host,
    unsigned server_port)
{
    try {
        HestiaApplication::initialize(
            config_path, user_token, extra_config, server_host, server_port);
    }
    catch (const std::exception& e) {
        set_last_error(e.what());
        return {
            OpStatus::Status::ERROR, hestia_error_t::HESTIA_ERROR_CLIENT_STATE,
            SOURCE_LOC() + " | Failed to initialize Hestia Application.\n"
                + e.what()};
    }
    return {};
}

void HestiaClient::clear_last_error()
{
    std::scoped_lock guard(m_mutex);
    m_last_errors[std::this_thread::get_id()].clear();
}

void HestiaClient::set_last_error(const std::string& msg)
{
    LOG_ERROR(msg);
    {
        std::scoped_lock guard(m_mutex);
        m_last_errors[std::this_thread::get_id()] = msg;
    }
}

void HestiaClient::get_last_error(std::string& error)
{
    std::scoped_lock guard(m_mutex);
    error = m_last_errors[std::this_thread::get_id()];
}

void HestiaClient::make_request(
    const HestiaRequest& request,
    completionFunc completion_func,
    Stream* stream,
    progressFunc progress_func) noexcept
{
    if (request.is_crud_request()) {
        HestiaResponse::Ptr response;
        try {
            response = do_crud(request);
        }
        catch (const std::exception& e) {
            response = HestiaResponse::create(
                {OpStatus::Status::ERROR, -1, "Exception in crud method"});
        }
        completion_func(std::move(response));
    }
    else {
        try {
            do_hsm_action(request, completion_func, stream, progress_func);
        }
        catch (const std::exception& e) {
            completion_func(HestiaResponse::create(
                {OpStatus::Status::ERROR, -1, "Exception in hsm action"}));
        }
    }
}

HestiaResponse::Ptr HestiaClient::do_crud(const HestiaRequest& request)
{
    clear_last_error();

    CrudService* service{nullptr};
    if (request.is_user_type()) {
        service = m_user_service.get();
    }
    else if (request.is_hsm_type()) {
        service = m_hsm_service;
    }
    if (service == nullptr) {
        return HestiaResponse::create(
            {OpStatus::Status::ERROR, -1, "Unsupported type requested."});
    }

    auto crud_response = service->make_request(
        CrudRequest{
            request.get_crud_request().method(),
            request.get_crud_request().get_query(),
            m_user_service->get_current_user_context()},
        request.get_hsm_type_as_string());

    return HestiaResponse::create(std::move(crud_response));
}

std::string HestiaClient::get_runtime_info() const
{
    return HestiaApplication::get_runtime_info();
}

void HestiaClient::do_hsm_action(
    const HestiaRequest& request,
    completionFunc completion_func,
    Stream* stream,
    progressFunc progress_func)
{
    (void)progress_func;
    clear_last_error();

    auto hsm_completion_func = [this, completion_func](
                                   HsmActionResponse::Ptr response) {
        if (!response->ok()) {
            set_last_error(
                "Error in hsm action: " + response->get_error().to_string());
        }
        completion_func(HestiaResponse::create(std::move(response)));
    };
    m_hsm_service->do_hsm_action(
        HsmActionRequest{
            request.get_action(), m_user_service->get_current_user_context()},
        stream, hsm_completion_func);
}

void HestiaClient::load_object_store_defaults()
{
    LOG_INFO("Loading fallback file based object store and tiers");
    ObjectStoreBackend backend(ObjectStoreBackend::Type::FILE_HSM);

    std::vector<std::string> tier_ids;
    for (uint8_t idx = 0; idx < 5; idx++) {

        StorageTier tier(std::to_string(1 + idx));
        tier.set_priority(idx);
        tier_ids.push_back(tier.id());
        m_config.add_storage_tier(tier);
    }
    backend.set_tier_ids(tier_ids);
    m_config.add_object_store_backend(backend);
}

void HestiaClient::set_app_mode(const std::string& host, unsigned port)
{
    if (!host.empty()) {
        m_config.override_controller_address(host, port);
    }

    if (!m_config.get_server_config().has_controller_address()) {
        LOG_INFO("Running CLIENT STANDALONE mode");
        m_app_mode = ApplicationMode::CLIENT_STANDALONE;
        if (!m_config.has_object_store_backends()) {
            LOG_INFO(
                "No object stores found in Standalone mode - adding a demo backend.");
            load_object_store_defaults();
        }
    }
    else {
        LOG_INFO("Running CLIENT FULL mode");
        m_app_mode = ApplicationMode::CLIENT_FULL;
    }
}

}  // namespace hestia