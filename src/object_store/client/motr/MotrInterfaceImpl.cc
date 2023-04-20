#include "MotrInterfaceImpl.h"

void MotrInterfaceImpl::initialize(const MotrConfig& config)
{
    mConfig = config;

    m0_config m0tr_config;
    m0tr_config.mc_is_oostore            = true;
    m0tr_config.mc_is_read_verify        = false;

    m0tr_config.mc_local_addr = mConfig.mLocalAddress.c_str();
    m0tr_config.mc_ha_addr = mConfig.mHaAddress.c_str();
    m0tr_config.mc_profile = mConfig.mProfile.c_str();
    m0tr_config.mc_process_fid = mConfig.mProcFid.c_str();

    const auto rc = m0_client_init(&mClientInstance, &m0tr_config, true);
    if (rc < 0) 
    {
        std::cerr << "m0 client init failed." << std::endl;
    }
}

void MotrInterfaceImpl::finish()
{
    if (mClientInstance)
    {
        m0_client_fini(mClientInstance, false);
        mClientInstance = nullptr;
    }
}

void MotrInterfaceImpl::initializeHsm()
{
    m0hsm_options hsm_options;
    hsm_options.trace_level = LOG_INFO;
    hsm_options.op_timeout = 10;
    hsm_options.log_stream = stderr;

    if (!std::filesystem::is_regular_file(working_path))
    {
        std::cerr << "No hsm config file found - bailing out." << std::endl;
        return;
    }

    FILE* f_in = ::fopen(working_path.c_str(), "r");
    hsm_options.rcfile = f_in;

	m0_container_init(&mContainer, nullptr, &M0_UBER_REALM, mClientInstance);
	auto rc = mContainer.co_realm.re_entity.en_sm.sm_rc;
	if (rc != 0) 
    {
        std::cerr << "m0_container_init() failed" << std::endl;
        return ;
	}
	mRealm = mContainer.co_realm;

    rc = m0hsm_init(mClientInstance, &mRealm, &hsm_options);
    if (rc < 0) 
    {
        std::cerr << "m0hsm: error: m0hsm_init() failed." << std::endl;
    }

    ::fclose(f_in);
}

int MotrInterfaceImpl::put(const hestia::hsm_uint& oid, const void* buf, const std::size_t length, 
    const hestia::tier_id_t target_tier)
{
    auto motr_obj = std::make_unique<MotrObject>(id);

    auto rc = m0hsm_create(motr_obj->getMotrFid(), &motr_obj->getMotrObj(), tier, false);
    if (rc < 0) 
    {
        std::cerr << "hsm obj create failed." << std::endl;
    }
}

int MotrInterfaceImpl::get(const hestia::hsm_uint& oid, void* buf, const std::size_t length,
    const hestia::tier_id_t src_tier)
{
    return 0;
}

int MotrInterfaceImpl::remove(const hestia::hsm_uint& oid, const hestia::tier_id_t tier)
{
    return 0;
}

int MotrInterfaceImpl::copy(const hestia::hsm_uint& oid, const hestia::tier_id_t src_tier, const hestia::tier_id_t tgt_tier)
{
    return 0;
}

int MotrInterfaceImpl::move(const hestia::hsm_uint& oid, const hestia::tier_id_t src_tier, const hestia::tier_id_t tgt_tier)
{
    return 0;
}