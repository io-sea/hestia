#pragma once

#include <string>
#include <vector>

namespace hestia {

struct CrudClientConfig {
    CrudClientConfig() = default;

    CrudClientConfig(
        const std::string& prefix, const std::string& endpoint = {}) :
        m_prefix(prefix), m_endpoint(endpoint)
    {
    }
    std::string m_prefix{"crud_client"};
    std::string m_endpoint;
};
}  // namespace hestia