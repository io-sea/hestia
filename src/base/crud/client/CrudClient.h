#pragma once

#include "CrudClientConfig.h"
#include "CrudRequest.h"
#include "CrudResponse.h"
#include "StringAdapter.h"

#include <memory>

namespace hestia {

class IdGenerator;
class TimeProvider;
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
        const CrudRequest& request, CrudResponse& response) const = 0;

    virtual void read(const CrudQuery& query, CrudResponse& response) const = 0;

    virtual void update(
        const CrudRequest& request, CrudResponse& response) const = 0;

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

  protected:
    bool matches_query(const Model& item, const Map& query) const;

    const StringAdapter* get_adapter(CrudAttributes::Format format) const;

    IdGenerator* m_id_generator;
    TimeProvider* m_time_provider;
    std::unique_ptr<IdGenerator> m_default_id_generator;
    std::unique_ptr<TimeProvider> m_default_time_provider;

    CrudClientConfig m_config;
    AdapterCollectionPtr m_adapters;
};
}  // namespace hestia