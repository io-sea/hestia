#pragma once

#ifdef __cplusplus
extern "C" {
#endif
//#include "hsm/m0hsm_api.h" // NOLINT
#ifdef __cplusplus
}
#endif

#include "IMotrInterfaceImpl.h"

#include <filesystem>

class MotrInterfaceImpl : public IMotrInterfaceImpl {
#ifdef HAS_MOTR
  public:
    void initialize(const MotrConfig& config);

    void copy(const HsmObjectStoreRequest& request) const override;

    void get(
        const HsmObjectStoreRequest& request,
        ostk::StorageObject& object,
        ostk::Stream* stream) const override;

    void move(const HsmObjectStoreRequest& request) const override;

    void put(const HsmObjectStoreRequest& request, ostk::Stream* stream)
        const override;

    void remove(const HsmObjectStoreRequest& request) const override;

  private:
    void finish();

    void initialize_hsm();

    MotrConfig m_config;
    m0_client* m_client_instance{nullptr};
    m0_container m_container;
    m0_realm m_realm;
#endif
};