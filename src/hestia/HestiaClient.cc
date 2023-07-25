#include "HestiaClient.h"

#include "HestiaConfig.h"
#include "hestia.h"

#include "DistributedHsmService.h"
#include "HsmService.h"
#include "HsmServicesFactory.h"
#include "UserService.h"

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
    const Dictionary& extra_config)
{
    try {
        HestiaApplication::initialize(config_path, user_token, extra_config);
    }
    catch (const std::exception& e) {
        set_last_error(e.what());
        return {
            OpStatus::Status::ERROR, hestia_error_t::HESTIA_ERROR_CLIENT_STATE,
            e.what()};
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

OpStatus HestiaClient::create(
    const HestiaType& subject,
    VecCrudIdentifier& ids,
    CrudAttributes& attributes,
    CrudAttributes::Format output_format)
{
    clear_last_error();

    CrudService* service{nullptr};
    if (subject.m_type == HestiaType::Type::SYSTEM
        && subject.m_system_type == HestiaType::SystemType::USER) {
        service = m_user_service.get();
    }
    else if (subject.m_type == HestiaType::Type::HSM) {
        service = m_hsm_service;
    }
    if (service != nullptr) {
        const auto response = service->make_request(
            CrudRequest{
                CrudMethod::CREATE, ids, attributes,
                CrudQuery::OutputFormat::ATTRIBUTES, output_format},
            HsmItem::to_name(subject.m_hsm_type));
        ERROR_CHECK(response, "CREATE");
        ids.clear();
        for (const auto& id : response->ids()) {
            ids.push_back(
                CrudIdentifier(id, CrudIdentifier::Type::PRIMARY_KEY));
        }
        attributes = response->attributes();
    }
    return {};
}

OpStatus HestiaClient::update(
    const HestiaType& subject,
    const VecCrudIdentifier& ids,
    CrudAttributes& attributes,
    CrudAttributes::Format output_format)
{
    clear_last_error();

    CrudService* service{nullptr};
    if (subject.m_type == HestiaType::Type::SYSTEM
        && subject.m_system_type == HestiaType::SystemType::USER) {
        service = m_user_service.get();
    }
    else if (subject.m_type == HestiaType::Type::HSM) {
        service = m_hsm_service;
    }
    if (service != nullptr) {
        const auto response = service->make_request(
            CrudRequest{
                CrudMethod::UPDATE, ids, attributes,
                CrudQuery::OutputFormat::ATTRIBUTES, output_format},
            HsmItem::to_name(subject.m_hsm_type));
        ERROR_CHECK(response, "UPDATE");
        attributes = response->attributes();
    }
    return {};
}

OpStatus HestiaClient::remove(
    const HestiaType& subject, const VecCrudIdentifier& ids)
{
    clear_last_error();

    CrudService* service{nullptr};
    if (subject.m_type == HestiaType::Type::SYSTEM
        && subject.m_system_type == HestiaType::SystemType::USER) {
        service = m_user_service.get();
    }
    else if (subject.m_type == HestiaType::Type::HSM) {
        service = m_hsm_service;
    }
    if (service != nullptr) {
        const auto response = service->make_request(
            CrudRequest{CrudMethod::REMOVE, ids},
            HsmItem::to_name(subject.m_hsm_type));
        ERROR_CHECK(response, "REMOVE");
    }
    return {};
}

OpStatus HestiaClient::read(const HestiaType& subject, CrudQuery& query)
{
    clear_last_error();

    // Serializeable::Format format = Serializeable::Format::CHILD_ID;
    CrudService* service{nullptr};
    if (subject.m_type == HestiaType::Type::SYSTEM) {
        if (subject.m_system_type == HestiaType::SystemType::USER) {
            service = m_user_service.get();
        }
        else if (subject.m_system_type == HestiaType::SystemType::HSM_NODE) {
            service = m_distributed_hsm_service->get_node_service();
        }
    }
    else if (subject.m_system_type == HestiaType::SystemType::HSM_NODE) {
        service = m_hsm_service;
    }

    if (service != nullptr) {
        CrudResponsePtr response;
        response = service->make_request(
            CrudRequest{query}, HsmItem::to_name(subject.m_hsm_type));
        ERROR_CHECK(response, "READ");
        query.attributes() = response->attributes();
    }
    return {};
}

void HestiaClient::do_data_io_action(
    HsmAction& action, Stream* stream, dataIoCompletionFunc completion_func)
{
    clear_last_error();

    auto hsm_completion_func =
        [this, completion_func](HsmActionResponse::Ptr response) {
            if (!response->ok()) {
                set_last_error(
                    "Error in data io operation: "
                    + response->get_error().to_string());
                completion_func(rc::error(response->get_error()), {});
            }
            else {
                completion_func({}, {});
            }
        };
    m_hsm_service->do_data_io_action(
        HsmActionRequest{action}, stream, hsm_completion_func);
}

OpStatus HestiaClient::do_data_movement_action(HsmAction& action)
{
    clear_last_error();

    if (const auto response = m_hsm_service->make_request({action});
        !response->ok()) {
        set_last_error(
            "Error in data movement operation: "
            + response->get_error().to_string());
        return rc::error(response->get_error());
    }
    return {};
}

void HestiaClient::load_object_store_defaults()
{
    HsmObjectStoreClientBackend backend(
        HsmObjectStoreClientBackend::Type::FILE_HSM,
        FileHsmObjectStoreClient::get_registry_identifier());
    m_config.add_object_store_backend(backend);

    for (uint8_t idx = 0; idx < 5; idx++) {
        StorageTier tier(idx);
        tier.set_backend(backend.get_identifier());
        m_config.add_storage_tier(tier);
    }
}

void HestiaClient::set_app_mode()
{
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