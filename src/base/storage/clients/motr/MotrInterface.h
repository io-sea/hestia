#pragma once

#include "IMotrInterfaceImpl.h"

#include "Uuid.h"

#include <filesystem>
#include <memory>
#include <string>

struct m0_obj;
struct m0_uint128;

class MotrObjImpl;

namespace hestia {
class MotrObject {
  public:
    MotrObject(const hestia::Uuid& oid);

    ~MotrObject();

    m0_uint128 get_motr_fid() const;

    m0_obj& get_motr_obj() const;

    static m0_uint128 to_motr_id(const hestia::Uuid& id);

  private:
    hestia::Uuid m_id;
    std::unique_ptr<MotrObjImpl> m_handle_impl;
};

class MotrInterface {
  public:
    MotrInterface(std::unique_ptr<IMotrInterfaceImpl> impl = nullptr);

    ~MotrInterface();

    void initialize(const MotrConfig& config);

    void copy(
        const HsmObjectStoreRequest& request,
        IMotrInterfaceImpl::completionFunc completion_func,
        IMotrInterfaceImpl::progressFunc progress_func) const;

    void get(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        IMotrInterfaceImpl::completionFunc completion_func,
        IMotrInterfaceImpl::progressFunc progress_func) const;

    void move(
        const HsmObjectStoreRequest& request,
        IMotrInterfaceImpl::completionFunc completion_func,
        IMotrInterfaceImpl::progressFunc progress_func) const;

    void put(
        const HsmObjectStoreRequest& request,
        Stream* stream,
        IMotrInterfaceImpl::completionFunc completion_func,
        IMotrInterfaceImpl::progressFunc progress_func) const;

    void remove(
        const HsmObjectStoreRequest& request,
        IMotrInterfaceImpl::completionFunc completion_func,
        IMotrInterfaceImpl::progressFunc progress_func) const;

  private:
    void validate_config(MotrConfig& config);

    void write_tier_info(
        const std::filesystem::path& path,
        const std::vector<MotrHsmTierInfo>& tier_info) const;

    std::unique_ptr<IMotrInterfaceImpl> m_impl;
};
}  // namespace hestia