#pragma once

#include "Extent.h"
#include "HsmItem.h"
#include "LockableModel.h"

#include <map>

namespace hestia {

class TierExtents : public HsmItem, public LockableModel {
  public:
    TierExtents();

    TierExtents(uint8_t tier_id, const Extent& extent);

    TierExtents(const TierExtents& other);

    virtual ~TierExtents();

    std::size_t get_size() const;

    static std::string get_type();

    const std::string& get_tier_id() const { return m_tier.get_id(); }

    const std::string& get_object_id() const { return m_object.get_id(); }

    const std::string& get_backend_id() const { return m_backend.get_id(); }

    void add_extent(const Extent& extent);

    bool empty() const;

    void remove_extent(const Extent& extent);

    void set_object_id(const std::string& id) { m_object.set_id(id); }

    void set_tier_id(const std::string& id) { m_tier.set_id(id); }

    void set_backend_id(const std::string& id) { m_backend.set_id(id); }

    void set_tier_name(uint8_t name) { m_tier_id.update_value(name); }

    const std::map<std::size_t, Extent>& get_extents() const
    {
        return m_extents.container();
    }

    TierExtents& operator=(const TierExtents& other);

  private:
    void init();

    UIntegerField m_tier_id{"tier_name", 0};
    IntKeyedSequenceField<std::map<std::size_t, Extent>> m_extents{
        "extents", "offset"};

    ForeignKeyField m_object{"object", HsmItem::hsm_object_name, true};
    ForeignKeyField m_tier{"tier", HsmItem::tier_name};
    ForeignKeyField m_backend{"backend", HsmItem::object_store_backend_name};
};
}  // namespace hestia