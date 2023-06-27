#pragma once

#include "HsmObjectStoreClientBackend.h"
#include "Model.h"

#include <string>
#include <vector>

namespace hestia {

class HsmNode : public Model {
  public:
    HsmNode();

    HsmNode(const Uuid& id);

    void deserialize(
        const Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL) override;

    void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const override;

    bool m_is_controller{false};

    std::string m_host_address;
    std::string m_port;
    std::string m_version;
    std::string m_app_type;
    std::vector<HsmObjectStoreClientBackend> m_backends;
};
}  // namespace hestia