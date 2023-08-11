#include "CrudClient.h"

#include "CrudService.h"
#include "IdGenerator.h"
#include "StringAdapter.h"
#include "TimeProvider.h"

#include "ErrorUtils.h"
#include "User.h"

#include <cassert>
#include <stdexcept>

namespace hestia {
CrudClient::CrudClient(
    const CrudClientConfig& config,
    AdapterCollectionPtr adapters,
    IdGenerator* id_generator,
    TimeProvider* time_provider) :
    m_id_generator(id_generator),
    m_time_provider(time_provider),
    m_config(config),
    m_adapters(std::move(adapters))
{
    if (m_id_generator == nullptr) {
        m_default_id_generator = std::make_unique<DefaultIdGenerator>();
        m_id_generator         = m_default_id_generator.get();
    }

    if (m_time_provider == nullptr) {
        m_default_time_provider = std::make_unique<DefaultTimeProvider>();
        m_time_provider         = m_default_time_provider.get();
    }
}

CrudClient::~CrudClient() {}

std::string CrudClient::generate_id(const std::string& name) const
{
    if (m_id_generator == nullptr) {
        throw std::runtime_error("ID Generation not supported for this client");
    }

    return m_id_generator->get_id(
        m_config.m_prefix + "::" + m_adapters->get_type() + "::" + name);
}

bool CrudClient::matches_query(const Model& item, const Map& query) const
{
    return m_adapters->get_default_adapter()->matches_query(item, query);
}

const StringAdapter* CrudClient::get_adapter(
    CrudAttributes::Format format) const
{
    return m_adapters->get_adapter(CrudAttributes::to_string(format));
}

std::string CrudClient::get_type() const
{
    return m_adapters->get_type();
}

Dictionary::Ptr CrudClient::create_child(
    const std::string& type,
    const std::string& parent_id,
    const CrudUserContext& user_context) const
{
    const auto child_service_iter = m_child_services.find(type);
    if (child_service_iter == m_child_services.end()) {
        THROW_WITH_SOURCE_LOC(
            "Attempted to find child service for type: " + type
            + " but it is not registered.");
    }

    CrudIdentifier id;
    id.set_parent_primary_key(parent_id);
    auto create_response = child_service_iter->second->make_request(CrudRequest{
        CrudMethod::CREATE,
        user_context,
        {id},
        {},
        CrudQuery::OutputFormat::DICT});
    if (!create_response->ok()) {
        throw std::runtime_error(
            "Failed to create default child: "
            + create_response->get_error().to_string());
    }
    return std::make_unique<Dictionary>(
        *create_response->dict()->get_sequence()[0]);
}

void CrudClient::get_or_create_default_parent(
    const std::string& type, const std::string& user_id)
{
    CrudIdentifier id;

    const auto parent_service_iter = m_parent_services.find(type);
    if (parent_service_iter == m_parent_services.end()) {
        THROW_WITH_SOURCE_LOC(
            "Attempted to find default parent for type: " + type
            + " but parent service not registered.");
    }

    if (type == User::get_type() && !user_id.empty()) {
        id = CrudIdentifier(user_id);
    }
    else {
        auto parent_default_name =
            parent_service_iter->second->get_default_name();
        if (parent_default_name.empty()) {
            THROW_WITH_SOURCE_LOC(
                "Attempted to find default parent for type: " + type
                + " but no default name set");
        }
        id = CrudIdentifier(parent_default_name, CrudIdentifier::Type::NAME);
    }

    auto get_response = parent_service_iter->second->make_request(
        CrudRequest{CrudQuery{id, CrudQuery::OutputFormat::ID}, user_id});
    if (!get_response->ok()) {
        throw std::runtime_error(
            "Failed to get default parent: "
            + get_response->get_error().to_string());
    }

    if (get_response->found()) {
        assert(!get_response->ids().empty());
        LOG_INFO(
            "Found default parent of type: " + type
            + " with id: " + get_response->ids()[0]);
        set_default_parent_id(type, get_response->ids()[0]);
    }
    else {
        auto create_response =
            parent_service_iter->second->make_request(CrudRequest{
                CrudMethod::CREATE,
                user_id,
                {id},
                {},
                CrudQuery::OutputFormat::ID});
        if (!create_response->ok()) {
            throw std::runtime_error(
                "Failed to create default parent: "
                + create_response->get_error().to_string());
        }
        assert(!create_response->ids().empty());
        LOG_INFO(
            "Created default parent of type: " + type
            + " with id: " + create_response->ids()[0]);
        set_default_parent_id(type, create_response->ids()[0]);
    }
}

std::string CrudClient::get_default_parent_id(const std::string& type) const
{
    if (auto iter = m_parent_default_ids.find(type);
        iter != m_parent_default_ids.end()) {
        return iter->second;
    }
    return {};
}

void CrudClient::register_parent_service(
    const std::string& type, CrudService* service)
{
    m_parent_services[type] = service;
}

void CrudClient::register_child_service(
    const std::string& type, CrudService* service)
{
    m_child_services[type] = service;
}

void CrudClient::set_default_parent_id(
    const std::string& type, const std::string& id)
{
    m_parent_default_ids[type] = id;
}

}  // namespace hestia