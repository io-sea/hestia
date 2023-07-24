#include "CrudClient.h"

#include "IdGenerator.h"
#include "StringAdapter.h"
#include "TimeProvider.h"

#include <stdexcept>

namespace hestia {
CrudClient::CrudClient(
    const CrudClientConfig& config,
    AdapterCollectionPtr adapters,
    IdGenerator* id_generator,
    TimeProvider* time_provider) :
    m_id_generator(id_generator),
    m_time_provider(time_provider),
    m_config(config),
    m_adapters(std::move(adapters))
{
    if (m_id_generator == nullptr) {
        m_default_id_generator = std::make_unique<DefaultIdGenerator>();
        m_id_generator         = m_default_id_generator.get();
    }

    if (m_time_provider == nullptr) {
        m_default_time_provider = std::make_unique<DefaultTimeProvider>();
        m_time_provider         = m_default_time_provider.get();
    }
}

CrudClient::~CrudClient() {}

std::string CrudClient::generate_id(const std::string& name) const
{
    if (m_id_generator == nullptr) {
        throw std::runtime_error("ID Generation not supported for this client");
    }

    return m_id_generator->get_id(
        m_config.m_prefix + "::" + m_adapters->get_type() + "::" + name);
}

bool CrudClient::matches_query(const Model& item, const Map& query) const
{
    return m_adapters->get_default_adapter()->matches_query(item, query);
}

const StringAdapter* CrudClient::get_adapter(
    CrudAttributes::Format format) const
{
    return m_adapters->get_adapter(CrudAttributes::to_string(format));
}

std::string CrudClient::get_type() const
{
    return m_adapters->get_type();
}

}  // namespace hestia