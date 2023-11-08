#include "CrudClient.h"

#include "CrudService.h"
#include "IdGenerator.h"
#include "TimeProvider.h"

#include "ErrorUtils.h"
#include "User.h"

#include <cassert>
#include <stdexcept>

namespace hestia {
CrudClient::CrudClient(
    const CrudClientConfig& config,
    ModelSerializer::Ptr model_serializer,
    IdGenerator* id_generator,
    TimeProvider* time_provider) :
    m_id_generator(id_generator),
    m_time_provider(time_provider),
    m_config(config),
    m_serializer(CrudSerializer::create(std::move(model_serializer)))
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
        m_config.m_prefix + "::" + m_serializer->get_type() + "::" + name);
}

bool CrudClient::matches_query(const Model& item, const Map& query) const
{
    return m_serializer->matches_query(item, query);
}

std::string CrudClient::get_type() const
{
    return m_serializer->get_type();
}

Dictionary::Ptr CrudClient::create_child(
    const std::string& type,
    const std::string& parent_id,
    const CrudUserContext& user_context) const
{
    const auto child_service_iter = m_child_services.find(type);
    if (child_service_iter == m_child_services.end()) {
        THROW(
            "Attempted to find child service for type: "
            << type << " but it is not registered.");
    }

    CrudIdentifier id;
    id.set_parent_primary_key(parent_id);
    auto create_response = child_service_iter->second->make_request(CrudRequest{
        CrudMethod::CREATE, {id, CrudQuery::BodyFormat::DICT}, user_context});
    if (!create_response->ok()) {
        THROW(
            "Failed to create default child: " << create_response->get_error());
    }
    return std::make_unique<Dictionary>(
        *create_response->get_attributes().get_dict()->get_sequence()[0]);
}

void CrudClient::remove_children(
    const std::unordered_map<std::string, std::vector<std::string>>& child_refs,
    const CrudUserContext& user_context) const
{
    for (const auto& [type, ids] : child_refs) {
        if (ids.empty()) {
            continue;
        }

        const auto child_service_iter = m_child_services.find(type);
        if (child_service_iter == m_child_services.end()) {
            THROW(
                "Attempted to find child service for type: "
                << type << " but it is not registered.");
        }

        auto remove_response =
            child_service_iter->second->make_request(CrudRequest{
                CrudMethod::REMOVE,
                {CrudIdentifierCollection(ids), CrudQuery::BodyFormat::NONE},
                user_context});
        if (!remove_response->ok()) {
            THROW(
                "Failed to remove children: " << remove_response->get_error());
        }
    }
}

std::string CrudClient::get_id_from_parent_id(
    const std::string& parent_type,
    const std::string& child_type,
    const std::string& id,
    const CrudUserContext& user_context) const
{
    const auto parent_service_iter = m_parent_services.find(parent_type);
    if (parent_service_iter == m_parent_services.end()) {
        THROW(
            "Attempted to find parent of type: "
            << parent_type << " but parent service not registered.");
    }

    auto get_response = parent_service_iter->second->make_request(CrudRequest{
        CrudMethod::READ, CrudQuery{id, CrudQuery::BodyFormat::ITEM},
        user_context});
    if (!get_response->ok()) {
        THROW("Failed to get default parent: " << get_response->get_error());
    }

    if (get_response->found()) {
        if (get_response->items().size() == 1) {
            return get_response->items()[0]->get_child_id_by_type(child_type);
        }
        else {
            LOG_WARN(
                "Unexpected number of return items in parent get: "
                << get_response->items().size())
        }
    }
    else {
        LOG_WARN("Failed to find parent object");
    }
    return {};
}

void CrudClient::get_or_create_default_parent(
    const std::string& type, const std::string& user_id)
{
    CrudIdentifier id;

    const auto parent_service_iter = m_parent_services.find(type);
    if (parent_service_iter == m_parent_services.end()) {
        THROW(
            "Attempted to find default parent for type: "
            << type << " but parent service not registered.");
    }

    if (type == User::get_type() && !user_id.empty()) {
        id = CrudIdentifier(user_id);
    }
    else {
        auto parent_default_name =
            parent_service_iter->second->get_default_name();
        if (parent_default_name.empty()) {
            THROW(
                "Attempted to find default parent for type: "
                << type << " but no default name set");
        }
        LOG_INFO(
            "Searching for default parent with name: " << parent_default_name);
        id = CrudIdentifier(parent_default_name, CrudIdentifier::Type::NAME);
        id.set_parent_primary_key(user_id);
    }

    auto get_response = parent_service_iter->second->make_request(CrudRequest{
        CrudMethod::READ, CrudQuery{id, CrudQuery::BodyFormat::ID}, user_id});
    if (!get_response->ok()) {
        THROW("Failed to get default parent: " << get_response->get_error());
    }

    if (get_response->found()) {
        assert(!get_response->ids().empty());
        LOG_INFO(
            "Found default parent of type: " + type
            + " with id: " + get_response->ids().first().get_primary_key());
        set_default_parent_id(
            type, get_response->ids().first().get_primary_key());
    }
    else {
        auto create_response =
            parent_service_iter->second->make_request(CrudRequest{
                CrudMethod::CREATE, {id, CrudQuery::BodyFormat::ID}, user_id});
        if (!create_response->ok()) {
            THROW(
                "Failed to create default parent: "
                << create_response->get_error());
        }
        assert(!create_response->ids().empty());
        LOG_INFO(
            "Created default parent of type: " + type
            + " with id: " + create_response->ids().first().get_primary_key());
        set_default_parent_id(
            type, create_response->ids().first().get_primary_key());
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