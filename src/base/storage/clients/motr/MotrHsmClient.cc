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

void MotrHsmClient::copy(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func) const
{
    m_motr_interface->copy(request, completion_func, progress_func);
}

void MotrHsmClient::get(
    const HsmObjectStoreRequest& request,
    Stream* stream,
    completionFunc completion_func,
    progressFunc progress_func) const
{
    m_motr_interface->get(request, stream, completion_func, progress_func);
}

void MotrHsmClient::move(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func) const
{
    m_motr_interface->move(request, completion_func, progress_func);
}

void MotrHsmClient::put(
    const HsmObjectStoreRequest& request,
    Stream* stream,
    completionFunc completion_func,
    progressFunc progress_func) const
{
    m_motr_interface->put(request, stream, completion_func, progress_func);
}

void MotrHsmClient::remove(
    const HsmObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func) const
{
    m_motr_interface->remove(request, completion_func, progress_func);
}
}  // namespace hestia