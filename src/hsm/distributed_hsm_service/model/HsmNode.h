#pragma once

#include "HsmObjectStoreClientBackend.h"

#include <string>
#include <vector>

namespace hestia {

class HsmNode {
  public:
    HsmNode(const std::string& id = {}) : m_id(id) {}

    const std::string& id() const { return m_id; }

    bool m_is_controller{false};

    std::string m_id;
    std::string m_host_address;
    std::string m_port;
    std::string m_tag;
    std::string m_version;
    std::string m_app_type;
    std::vector<HsmObjectStoreClientBackend> m_backends;
};
}  // namespace hestia