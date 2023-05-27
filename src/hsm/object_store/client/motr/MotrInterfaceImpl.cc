#include "MotrInterfaceImpl.h"

#include <iostream>

#ifdef HAS_MOTR
namespace hestia {
void MotrInterfaceImpl::initialize(const MotrConfig& config)
{
    m_config = config;

    m0_config m0tr_config;
    m0tr_config.mc_is_oostore     = true;
    m0tr_config.mc_is_read_verify = false;

    m0tr_config.mc_local_addr  = m_config.m_local_address.c_str();
    m0tr_config.mc_ha_addr     = m_config.m_ha_address.c_str();
    m0tr_config.mc_profile     = m_config.m_profile.c_str();
    m0tr_config.mc_process_fid = m_config.m_proc_fid.c_str();

    const auto rc = m0_client_init(&m_client_instance, &m0tr_config, true);
    if (rc < 0) {
        std::cerr << "m0 client init failed." << std::endl;
    }
}

void MotrInterfaceImpl::finish()
{
    if (m_client_instance) {
        m0_client_fini(m_client_instance, false);
        m_client_instance = nullptr;
    }
}

void MotrInterfaceImpl::initialize_hsm()
{
    m0hsm_options hsm_options;
    hsm_options.trace_level = LOG_INFO;
    hsm_options.op_timeout  = 10;
    hsm_options.log_stream  = stderr;

    if (!std::filesystem::is_regular_file(m_config.m_hsm_config_path)) {
        std::cerr << "No hsm config file found - bailing out." << std::endl;
        return;
    }

    FILE* f_in         = ::fopen(m_config.m_hsm_config_path.c_str(), "r");
    hsm_options.rcfile = f_in;

    m0_container_init(&m_container, nullptr, &M0_UBER_REALM, m_client_instance);
    auto rc = m_container.co_realm.re_entity.en_sm.sm_rc;
    if (rc != 0) {
        std::cerr << "m0_container_init() failed" << std::endl;
        return;
    }
    m_realm = m_container.co_realm;

    rc = m0hsm_init(m_client_instance, &m_realm, &hsm_options);
    if (rc < 0) {
        std::cerr << "m0hsm: error: m0hsm_init() failed." << std::endl;
    }

    ::fclose(f_in);
}

int MotrInterfaceImpl::put(
    const HsmObjectStoreRequest& request, hestia::Stream* stream)
{
    auto motr_obj = std::make_unique<MotrObject>(id);

    auto rc = m0hsm_create(
        motr_obj->getMotrFid(), &motr_obj->getMotrObj(), tier, false);
    if (rc < 0) {
        std::cerr << "hsm obj create failed." << std::endl;
    }
}

int MotrInterfaceImpl::get(
    const HsmObjectStoreRequest& request,
    hestia::StorageObject& object,
    hestia::Stream* stream)
{
    return 0;
}

int MotrInterfaceImpl::remove(const HsmObjectStoreRequest& request)
{
    return 0;
}

int MotrInterfaceImpl::copy(const HsmObjectStoreRequest& request)
{
    return 0;
}

int MotrInterfaceImpl::move(const HsmObjectStoreRequest& request)
{
    return 0;
}
}  // namespace hestia
#endif