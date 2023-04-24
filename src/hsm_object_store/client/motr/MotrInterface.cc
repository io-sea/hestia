#include "MotrInterface.h"

#ifdef HAS_MOTR_IMPL
#include "MotrInterfaceImpl.h"
#endif

#include <fstream>

MotrInterface::MotrInterface(std::unique_ptr<IMotrInterfaceImpl> impl) :
    m_impl(std::move(impl))
{
#ifdef HAS_MOTR_IMPL
    if (!m_impl) {
        m_impl = std::make_unique<MotrInterfaceImpl>();
    }
#endif
}

MotrInterface::~MotrInterface() {}

void MotrInterface::initialize(const MotrConfig& config)
{
    MotrConfig working_config = config;
    validate_config(working_config);
    m_impl->initialize(working_config);
}

void MotrInterface::validate_config(MotrConfig& config)
{
    auto working_path = config.m_hsm_config_path;
    if (working_path.empty()) {
        working_path = std::filesystem::current_path() / "config";
        write_tier_info(working_path, config.m_tier_info);
    }
    config.m_hsm_config_path = working_path;
}

void MotrInterface::copy(const HsmObjectStoreRequest& request) const
{
    m_impl->copy(request);
}

void MotrInterface::get(
    const HsmObjectStoreRequest& request,
    ostk::StorageObject& object,
    ostk::Stream* stream) const
{
    m_impl->get(request, object, stream);
}

void MotrInterface::move(const HsmObjectStoreRequest& request) const
{
    m_impl->move(request);
}

void MotrInterface::put(
    const HsmObjectStoreRequest& request, ostk::Stream* stream) const
{
    m_impl->put(request, stream);
}

void MotrInterface::remove(const HsmObjectStoreRequest& request) const
{
    m_impl->remove(request);
}

void MotrInterface::write_tier_info(
    const std::filesystem::path& path,
    const std::vector<MotrHsmTierInfo>& tier_info) const
{
    std::ofstream f_out;
    f_out.open(path);
    for (const auto& tier : tier_info) {
        f_out << tier.m_name << " = <" << tier.m_identifier << "> \n";
    }
    f_out.close();
}