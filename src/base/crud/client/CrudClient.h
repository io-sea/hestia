#pragma once

#include "CrudClientConfig.h"
#include "CrudRequest.h"
#include "CrudResponse.h"
#include "StringAdapter.h"

#include <memory>
#include <unordered_map>

namespace hestia {

class IdGenerator;
class TimeProvider;
class CrudService;

using AdapterCollectionPtr = std::unique_ptr<AdapterCollection>;

class CrudClient {
  public:
    CrudClient(
        const CrudClientConfig& config,
        AdapterCollectionPtr adapters,
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

    virtual void remove(const VecCrudIdentifier& ids) const = 0;

    virtual void identify(
        const VecCrudIdentifier& ids, CrudResponse& response) const = 0;

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

    const StringAdapter* get_adapter(CrudAttributes::Format format) const;

    void get_or_create_default_parent(
        const std::string& type, const std::string& user_id);

    Dictionary::Ptr create_child(
        const std::string& type,
        const std::string& parent_id,
        const CrudUserContext& user_context) const;

    std::string get_default_parent_id(const std::string& type) const;

    void set_default_parent_id(const std::string& type, const std::string& id);

    IdGenerator* m_id_generator;
    TimeProvider* m_time_provider;
    std::unique_ptr<IdGenerator> m_default_id_generator;
    std::unique_ptr<TimeProvider> m_default_time_provider;

    CrudClientConfig m_config;
    AdapterCollectionPtr m_adapters;

    std::unordered_map<std::string, std::string> m_parent_default_ids;
    std::unordered_map<std::string, CrudService*> m_parent_services;
    std::unordered_map<std::string, CrudService*> m_child_services;
};
}  // namespace hestia