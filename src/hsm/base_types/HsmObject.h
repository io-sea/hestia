#pragma once

#include "HsmItem.h"
#include "LockableModel.h"
#include "TierExtents.h"
#include "UserMetadata.h"

namespace hestia {
class HsmObject : public HsmItem, public LockableModel {
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

    void set_content_modified_time(std::time_t t);

    void set_content_accessed_time(std::time_t t);

    const std::vector<TierExtents>& tiers() const;

    HsmObject& operator=(const HsmObject& other);

  private:
    void init();

    UIntegerField m_size{"size"};

    OneToOneProxyField<UserMetadata> m_metadata{"user_metadata", true};
    ForeignKeyProxyField<TierExtents> m_tier_extents{"tiers"};
    ForeignKeyField m_dataset{"dataset", HsmItem::dataset_name};

    DateTimeField m_content_modified_time{"content_modified_time"};
    DateTimeField m_content_accessed_time{"content_accessed_time"};
};
}  // namespace hestia
