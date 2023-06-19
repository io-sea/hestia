#pragma once

#include <string>

namespace hestia {

class StorageTier {
  public:
    enum class tier_storage_class { ssd_nvme, ssd_sata, hdd, tape };

    StorageTier() = default;

    StorageTier(const std::string& id) : m_id(id) {}

    StorageTier(uint8_t id) : m_id(std::to_string(id)) {}

    const std::string& id() const { return m_id; }

    uint8_t id_uint() const
    {
        if (m_id.empty()) {
            return 0;
        }
        return std::stoul(m_id);
    }

    tier_storage_class m_type{tier_storage_class::ssd_nvme};
    std::size_t m_capacity{0};
    std::size_t m_bandwith{0};
    std::string m_id;
    std::string m_backend;
};
}  // namespace hestia
