#include "MotrInterface.h"

#ifdef HAS_MOTR_IMPL
#include "MotrInterfaceImpl.h"
#endif

#include "Logger.h"

#include <fstream>

namespace hestia {
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
    auto working_path = config.m_hsm_config_path.get_value();
    if (working_path.empty()) {
        working_path = std::filesystem::current_path() / "config";
        write_tier_info(working_path, config.m_tier_info.container());
    }
    config.m_hsm_config_path.update_value(working_path);
}

void MotrInterface::copy(
    const HsmObjectStoreRequest& request,
    IMotrInterfaceImpl::completionFunc completion_func,
    IMotrInterfaceImpl::progressFunc progress_func) const
{
    m_impl->copy(request, completion_func, progress_func);
}

void MotrInterface::get(
    const HsmObjectStoreRequest& request,
    Stream* stream,
    IMotrInterfaceImpl::completionFunc completion_func,
    IMotrInterfaceImpl::progressFunc progress_func) const
{
    m_impl->get(request, stream, completion_func, progress_func);
}

void MotrInterface::move(
    const HsmObjectStoreRequest& request,
    IMotrInterfaceImpl::completionFunc completion_func,
    IMotrInterfaceImpl::progressFunc progress_func) const
{
    m_impl->move(request, completion_func, progress_func);
}

void MotrInterface::put(
    const HsmObjectStoreRequest& request,
    Stream* stream,
    IMotrInterfaceImpl::completionFunc completion_func,
    IMotrInterfaceImpl::progressFunc progress_func) const
{
    m_impl->put(request, stream, completion_func, progress_func);
}

void MotrInterface::remove(
    const HsmObjectStoreRequest& request,
    IMotrInterfaceImpl::completionFunc completion_func,
    IMotrInterfaceImpl::progressFunc progress_func) const
{
    m_impl->remove(request, completion_func, progress_func);
}

void MotrInterface::set_tier_ids(const std::vector<std::string>& ids)
{
    LOG_INFO("Setting tier ids");
    m_impl->set_tier_ids(ids);
}

void MotrInterface::write_tier_info(
    const std::filesystem::path& path,
    const std::vector<MotrHsmTierInfo>& tier_info) const
{
    std::ofstream f_out;
    f_out.open(path);
    for (const auto& tier : tier_info) {
        f_out << tier.m_name.get_value() << " = <"
              << tier.m_identifier.get_value() << "> \n";
    }
    f_out.close();
}
}  // namespace hestia