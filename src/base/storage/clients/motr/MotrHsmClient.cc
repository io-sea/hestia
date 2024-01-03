#include "MotrHsmClient.h"

#include "MotrInterface.h"
#include "StringUtils.h"

namespace hestia {
MotrHsmClient::MotrHsmClient(std::unique_ptr<MotrInterface> interface) :
    m_motr_interface(std::move(interface))
{
    if (m_motr_interface == nullptr) {
        m_motr_interface = std::make_unique<MotrInterface>();
    }
}

void MotrHsmClient::initialize(
    const std::string& id,
    const std::string& cache_path,
    const Dictionary& data)
{
    MotrConfig config;
    config.deserialize(data);
    do_intialize(id, cache_path, config);
}

void MotrHsmClient::do_intialize(
    const std::string& id, const std::string&, const MotrConfig& config)
{
    m_id = id;
    m_motr_interface->initialize(config);
}

void MotrHsmClient::set_tier_ids(const std::vector<std::string>& tier_ids)
{
    m_tier_ids = tier_ids;
    m_motr_interface->set_tier_ids(tier_ids);
}

void MotrHsmClient::copy(HsmObjectStoreContext& ctx) const
{
    m_motr_interface->copy(
        ctx.m_request, ctx.m_completion_func, ctx.m_progress_func);
}

void MotrHsmClient::get(HsmObjectStoreContext& ctx) const
{
    m_motr_interface->get(
        ctx.m_request, ctx.m_stream, ctx.m_completion_func,
        ctx.m_progress_func);
}

void MotrHsmClient::move(HsmObjectStoreContext& ctx) const
{
    m_motr_interface->move(
        ctx.m_request, ctx.m_completion_func, ctx.m_progress_func);
}

void MotrHsmClient::put(HsmObjectStoreContext& ctx) const
{
    m_motr_interface->put(
        ctx.m_request, ctx.m_stream, ctx.m_completion_func,
        ctx.m_progress_func);
}

void MotrHsmClient::remove(HsmObjectStoreContext& ctx) const
{
    m_motr_interface->remove(
        ctx.m_request, ctx.m_completion_func, ctx.m_progress_func);
}
}  // namespace hestia