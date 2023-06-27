#pragma once

#include "Model.h"
#include <string>

namespace hestia {

class StorageTier : public Model {
  public:
    enum class medium { ssd_nvme, ssd_sata, hdd, tape };

    StorageTier();

    StorageTier(const std::string& id);

    StorageTier(uint8_t id);

    uint8_t id_uint() const;

    void deserialize(
        const Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL) override;

    void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const override;

    medium m_medium{medium::ssd_nvme};
    std::size_t m_capacity{0};
    std::size_t m_bandwith{0};
    std::string m_backend;
};
}  // namespace hestia
