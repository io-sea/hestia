#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "hsm/m0hsm_api.h"
#ifdef __cplusplus
}
#endif

#include "IMotrInterfaceImpl.h"

#include <filsystem>

class MotrInterfaceImpl : public IMotrInterfaceImpl
{
public:
    void initialize(const MotrConfig& config);

    void copy(const HsmObjectStoreRequest& request) const override;

    void get(const HsmObjectStoreRequest& request, ostk::StorageObject& object, ostk::Stream* stream) const override;

    void move(const HsmObjectStoreRequest& request) const override;

    void put(const HsmObjectStoreRequest& request, ostk::Stream* stream) const override;

    void remove(const HsmObjectStoreRequest& request) const override;
private:
    void finish();

    void initializeHsm();

    MotrConfig mConfig;
    m0_client* mClientInstance{nullptr};
    m0_container mContainer;
    m0_realm mRealm;   
};