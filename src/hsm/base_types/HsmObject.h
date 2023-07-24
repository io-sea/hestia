#pragma once

#include "HsmItem.h"
#include "LockableOwnableModel.h"
#include "TierExtents.h"
#include "UserMetadata.h"

namespace hestia {
class HsmObject : public HsmItem, public LockableOwnableModel {
  public:
    HsmObject();

    HsmObject(const std::string& id);

    HsmObject(const HsmObject& other);

    static std::string get_type();

    const std::string& dataset() const;

    const Map& metadata() const;

    std::string to_string() const;

    void set_size(std::size_t size);

    std::size_t size() const;

    void set_dataset_id(const std::string& id);

    const std::vector<TierExtents>& tiers() const;

    HsmObject& operator=(const HsmObject& other);

  private:
    void init();

    UIntegerField m_size{"size"};
    OneToOneProxyField<UserMetadata> m_metadata{"user_metadata"};
    ForeignKeyProxyField<TierExtents> m_tier_extents{"tiers"};
    NamedForeignKeyField m_dataset{"dataset", HsmItem::dataset_name};
};
}  // namespace hestia
