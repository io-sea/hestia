#pragma once

#ifdef HAS_MOTR

#ifdef __cplusplus
extern "C" {
#endif
#include "hsm/m0hsm_api.h"  // NOLINT
#ifdef __cplusplus
}
#endif

#include "IMotrInterfaceImpl.h"

#include <filesystem>

namespace hestia {
class MotrInterfaceImpl : public IMotrInterfaceImpl {
  public:
    void initialize(const MotrConfig& config) override;

    void copy(const HsmObjectStoreRequest& request) const override;

    void get(
        const HsmObjectStoreRequest& request,
        hestia::StorageObject& object,
        hestia::Stream* stream) const override;

    void move(const HsmObjectStoreRequest& request) const override;

    void put(const HsmObjectStoreRequest& request, hestia::Stream* stream)
        const override;

    void remove(const HsmObjectStoreRequest& request) const override;

  private:
    void finish();

    void initialize_hsm(const std::vector<MotrHsmTierInfo>& tier_info) override;

    MotrConfig m_config;
    struct m0_config m_m0tr_config;
    struct m0_idx_dix_config m_dix_conf;
    m0_client* m_client_instance{nullptr};
    m0_container m_container;
    m0_realm m_realm;
};
}  // namespace hestia

#endif