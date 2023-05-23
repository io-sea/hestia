#include "HsmObjectStoreClientSpec.h"

namespace hestia {
HsmObjectStoreClientSpec::HsmObjectStoreClientSpec(
    Type client_type, Source source, const std::string& identifier) :
    m_type(client_type), m_source(source), m_identifier(identifier)
{
}

HsmObjectStoreClientSpec::HsmObjectStoreClientSpec(
    const HsmObjectStoreClientSpec& other)
{
    m_type         = other.m_type;
    m_source       = other.m_source;
    m_identifier   = other.m_identifier;
    m_extra_config = other.m_extra_config;
}

HsmObjectStoreClientSpec::HsmObjectStoreClientSpec(const Dictionary& config)
{
    if (const auto identifier_dict = config.get_map_item("identifier")) {
        m_identifier = identifier_dict->get_scalar();
    }

    if (const auto source_dict = config.get_map_item("source")) {
        set_source(source_dict->get_scalar());
    }

    if (const auto type_dict = config.get_map_item("type")) {
        set_type(type_dict->get_scalar());
    }
    config.get_map_items(m_extra_config, {"identifier", "source", "type"});
}

void HsmObjectStoreClientSpec::set_source(const std::string& source)
{
    if (source == "BUILT_IN") {
        m_source = Source::BUILT_IN;
    }
    else if (source == "PLUGIN") {
        m_source = Source::PLUGIN;
    }
    else if (source == "MOCK") {
        m_source = Source::MOCK;
    }
}

void HsmObjectStoreClientSpec::set_type(const std::string& type)
{
    if (type == "Basic") {
        m_type = Type::BASIC;
    }
    else if (type == "HSM") {
        m_type = Type::HSM;
    }
}

bool HsmObjectStoreClientSpec::is_hsm() const
{
    return m_type == Type::HSM;
}

std::string HsmObjectStoreClientSpec::to_string() const
{
    const std::string type_str = m_type == Type::BASIC ? "Basic" : "HSM";
    std::string source_str;
    switch (m_source) {
        case Source::BUILT_IN:
            source_str = "BUILT_IN";
            break;
        case Source::PLUGIN:
            source_str = "PLUGIN";
            break;
        case Source::MOCK:
            source_str = "MOCK";
            break;
    }
    return "Type: " + type_str + " | Source: " + source_str
           + " | Identifier: " + m_identifier;
}
}  // namespace hestia