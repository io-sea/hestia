#pragma once

#include "Dictionary.h"

#include <string>

namespace hestia {
class HsmObjectStoreClientBackend {
  public:
    enum class Type { HSM, BASIC };

    enum class Source { BUILT_IN, PLUGIN, MOCK };

    HsmObjectStoreClientBackend() = default;

    HsmObjectStoreClientBackend(
        Type client_type, Source source, const std::string& identifier);

    HsmObjectStoreClientBackend(
        const Dictionary& config, const std::string& cache_path = "");

    HsmObjectStoreClientBackend(const HsmObjectStoreClientBackend& other);

    void set_source(const std::string& source);

    void set_type(const std::string& type);

    Dictionary::Ptr serialize() const;

    void deserialize(const Dictionary& dict);

    bool is_hsm() const;

    std::string to_string() const;

    HsmObjectStoreClientBackend& operator=(
        const HsmObjectStoreClientBackend& other)
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