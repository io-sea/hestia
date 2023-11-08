#pragma once

#include "IMotrInterfaceImpl.h"
#include "MockMotrHsm.h"

#include <memory>
#include <unordered_map>

namespace hestia {
class MotrObject;

class MockMotrInterfaceImpl : public IMotrInterfaceImpl {
  public:
    virtual ~MockMotrInterfaceImpl() = default;

    void initialize(const MotrConfig& config) override;

    void copy(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void get(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void move(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void put(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void remove(
        const HsmObjectStoreRequest& request,
        completionFunc completion_func,
        progressFunc progress_func) const override;

    void initialize_hsm(const std::vector<MotrHsmTierInfo>&) override;

    mock::motr::Hsm m_hsm;
    mock::motr::Client m_client;
    mock::motr::Container m_container;
};
}  // namespace hestia