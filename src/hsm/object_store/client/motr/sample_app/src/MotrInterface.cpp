#include "MotrInterface.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "hsm/m0hsm_api.h"
#ifdef __cplusplus
}
#endif

#include <iostream>

m0_client* MotrInterface::getClientInstance() const
{
    return mClientInstance;
}

void MotrInterface::initializeClientInstance(const ClientConfig& config)
{
    mClientConfig = config;

    m0_config m0tr_config;
    m0tr_config.mc_is_oostore            = true;
    m0tr_config.mc_is_read_verify        = false;

    m0tr_config.mc_local_addr = mClientConfig.mLocalAddress.c_str();
    m0tr_config.mc_ha_addr = mClientConfig.mHaAddress.c_str();
    m0tr_config.mc_profile = mClientConfig.mProfile.c_str();
    m0tr_config.mc_process_fid = mClientConfig.mProcFid.c_str();

    const auto rc = m0_client_init(&mClientInstance, &m0tr_config, true);
    if (rc < 0) 
    {
        std::cerr << "m0 client init failed." << std::endl;
    }
}

void MotrInterface::initializeHsm()
{
    m0hsm_options hsm_options;
    hsm_options.trace_level = LOG_INFO;
    hsm_options.op_timeout = 10;
    hsm_options.log_stream = stderr;

    m0_realm uber_realm;

    const auto rc = m0hsm_init(mClientInstance, &uber_realm, &hsm_options);
    if (rc < 0) 
    {
        std::cerr << "m0hsm: error: m0hsm_init() failed." << std::endl;
    }
}