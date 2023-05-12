#pragma once

#include "Dictionary.h"

#include <string>

namespace hestia {
class HsmObjectStoreClientSpec {
  public:
    enum class Type { HSM, BASIC };

    enum class Source { BUILT_IN, PLUGIN, MOCK };

    HsmObjectStoreClientSpec(
        Type client_type, Source source, const std::string& identifier);

    HsmObjectStoreClientSpec(const Dictionary& config);

    HsmObjectStoreClientSpec(const HsmObjectStoreClientSpec& other)
    {
        m_type         = other.m_type;
        m_source       = other.m_source;
        m_identifier   = other.m_identifier;
        m_extra_config = other.m_extra_config;
    }

    void set_source(const std::string& source);

    void set_type(const std::string& type);

    bool is_hsm() const;

    std::string to_string() const;

    HsmObjectStoreClientSpec& operator=(const HsmObjectStoreClientSpec& other)
    {
        if (this != &other) {
            m_type         = other.m_type;
            m_source       = other.m_source;
            m_identifier   = other.m_identifier;
            m_extra_config = other.m_extra_config;
        }
        return *this;
    }

    Type m_type{Type::BASIC};
    Source m_source{Source::BUILT_IN};
    std::string m_identifier;
    Metadata m_extra_config;
};
}  // namespace hestia