#pragma once

#include "CrudClientConfig.h"
#include "CrudSerializer.h"

#include <unordered_map>

namespace hestia {

class IdGenerator;
class TimeProvider;
class CrudService;

class CrudClient {
  public:
    CrudClient(
        const CrudClientConfig& config,
        ModelSerializer::Ptr serializer,
        IdGenerator* id_generator   = nullptr,
        TimeProvider* time_provider = nullptr);

    using Ptr = std::unique_ptr<CrudClient>;

    virtual ~CrudClient();

    virtual void create(
        const CrudRequest& request,
        CrudResponse& response,
        bool record_modified_attrs = false) = 0;

    virtual void read(
        const CrudRequest& request, CrudResponse& response) const = 0;

    virtual void update(
        const CrudRequest& request,
        CrudResponse& response,
        bool record_modified_attrs = false) const = 0;

    virtual void remove(
        const CrudRequest& request, CrudResponse& response) const = 0;

    virtual void identify(
        const CrudRequest& request, CrudResponse& response) const = 0;

    virtual bool is_locked(
        const CrudIdentifier& id, CrudLockType lock_type) const = 0;

    virtual void lock(
        const CrudIdentifier& id, CrudLockType lock_type) const = 0;

    virtual void unlock(
        const CrudIdentifier& id, CrudLockType lock_type) const = 0;

    virtual std::string generate_id(const std::string& name) const;

    std::string get_type() const;

    void register_parent_service(const std::string& type, CrudService* service);

    void register_child_service(const std::string& type, CrudService* service);

  protected:
    bool matches_query(const Model& item, const Map& query) const;

    void get_or_create_default_parent(
        const std::string& type, const std::string& user_id);

    Dictionary::Ptr create_child(
        const std::string& type,
        const std::string& parent_id,
        const CrudUserContext& user_context) const;

    std::string get_default_parent_id(const std::string& type) const;

    void set_default_parent_id(const std::string& type, const std::string& id);

    std::string get_id_from_parent_id(
        const std::string& parent_type,
        const std::string& child_type,
        const std::string& id,
        const CrudUserContext& user_context) const;

    IdGenerator* m_id_generator;
    TimeProvider* m_time_provider;
    std::unique_ptr<IdGenerator> m_default_id_generator;
    std::unique_ptr<TimeProvider> m_default_time_provider;

    CrudClientConfig m_config;
    CrudSerializer::Ptr m_serializer;

    std::unordered_map<std::string, std::string> m_parent_default_ids;
    std::unordered_map<std::string, CrudService*> m_parent_services;
    std::unordered_map<std::string, CrudService*> m_child_services;
};
}  // namespace hestia