#pragma once

#include <string>

namespace hestia {

class ObjectStoreBackend {
  public:
    std::string m_identifier;
};

class HestiaNode {
  public:
    HestiaNode(const std::string& id = {}) : m_id(id) {}

    const std::string& id() const { return m_id; }

    bool m_is_controller{false};

    std::string m_id;
    std::string m_host_address;
    std::string m_tag;
    std::string m_version;
    std::string m_app_type;
};
}  // namespace hestia