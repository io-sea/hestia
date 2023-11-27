#pragma once

#include "Extent.h"
#include "HsmItem.h"
#include "LockableModel.h"

#include <map>

namespace hestia {

class TierExtents : public HsmItem, public LockableModel {
  public:
    TierExtents();

    TierExtents(uint8_t tier_priority, const Extent& extent);

    TierExtents(const TierExtents& other);

    virtual ~TierExtents();

    void add_extent(const Extent& extent);

    bool empty() const;

    const std::map<std::size_t, Extent>& get_extents() const;

    std::size_t get_size() const;

    static std::string get_type();

    const std::string& get_tier_id() const;

    const std::string& get_object_id() const;

    const std::string& get_backend_id() const;

    void remove_extent(const Extent& extent);

    void set_object_id(const std::string& id);

    void set_tier_id(const std::string& id);

    void set_backend_id(const std::string& id);

    void set_tier_priority(uint8_t priority);

    TierExtents& operator=(const TierExtents& other);

  private:
    void init();

    UIntegerField m_tier_priority{"tier_priority", 0};
    IntKeyedSequenceField<std::map<std::size_t, Extent>> m_extents{
        "extents", "offset"};

    ForeignKeyField m_object{"object", HsmItem::hsm_object_name, true};
    ForeignKeyField m_tier{"tier", HsmItem::tier_name};
    ForeignKeyField m_backend{"backend", HsmItem::object_store_backend_name};
};
}  // namespace hestia