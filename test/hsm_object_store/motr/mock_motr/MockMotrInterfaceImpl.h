#pragma once

#include "IMotrInterfaceImpl.h"
#include "MockMotrHsm.h"

#include <memory>
#include <unordered_map>

class MotrObject;

class MockMotrInterfaceImpl : public IMotrInterfaceImpl {
  public:
    virtual ~MockMotrInterfaceImpl() = default;

    void initialize(const MotrConfig& config) override;

    void copy(const HsmObjectStoreRequest& request) const override;

    void get(
        const HsmObjectStoreRequest& request,
        ostk::StorageObject& object,
        ostk::Stream* stream) const override;

    void move(const HsmObjectStoreRequest& request) const override;

    void put(const HsmObjectStoreRequest& request, ostk::Stream* stream)
        const override;

    void remove(const HsmObjectStoreRequest& request) const override;

    void initialize_hsm(const std::vector<MotrHsmTierInfo>&) override;

    mock::motr::Hsm m_hsm;
    mock::motr::Client m_client;
    mock::motr::Container m_container;
};