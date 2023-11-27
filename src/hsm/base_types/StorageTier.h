#pragma once

#include "HsmItem.h"
#include "ObjectStoreBackend.h"
#include "TierExtents.h"
#include <string>

namespace hestia {

class StorageTier : public HsmItem, public Model {
  public:
    StorageTier();

    StorageTier(const std::string& id);

    StorageTier(const StorageTier& other);

    static std::string get_type();

    const std::vector<ObjectStoreBackend>& get_backends() const;

    const std::vector<TierExtents>& get_extents() const;

    std::size_t get_capacity() const;

    std::size_t get_bandwidth() const;

    uint8_t get_priority() const;

    void set_capacity(std::size_t capacity);

    void set_bandwidth(std::size_t bandwidth);

    void set_priority(uint8_t priority);

    StorageTier& operator=(const StorageTier& other);

  private:
    void init();

    UIntegerField m_priority{"priority", 255};
    UIntegerField m_capacity{"capacity"};
    UIntegerField m_bandwidth{"bandwidth"};
    ForeignKeyProxyField<ObjectStoreBackend> m_backends{"backends"};
    ForeignKeyProxyField<TierExtents> m_tier_extents{"extents"};
};
}  // namespace hestia
