#pragma once

#include <string>

namespace hestia {

class StorageTier {
  public:
    enum class tier_storage_class { ssd_nvme, ssd_sata, hdd, tape };

    StorageTier() = default;

    StorageTier(const std::string& id) : m_id(id) {}

    const std::string& id() const { return m_id; }

    tier_storage_class m_type{tier_storage_class::ssd_nvme};
    std::size_t capacity{0};
    std::size_t bandwith{0};
    std::string m_id;
    std::string m_backend;
};
}  // namespace hestia