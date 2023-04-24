#pragma once

#include "IMotrInterfaceImpl.h"

#include <ostk/Uuid.h>

#include <filesystem>
#include <memory>
#include <string>

struct m0_obj;
struct m0_uint128;

class MotrObjImpl;

class MotrObject {
  public:
    MotrObject(const ostk::Uuid& oid);

    ~MotrObject();

    m0_uint128 get_motr_fid() const;

    m0_obj& get_motr_obj() const;

    static m0_uint128 to_motr_id(const ostk::Uuid& id);

  private:
    ostk::Uuid m_id;
    std::unique_ptr<MotrObjImpl> m_handle_impl;
};

class MotrInterface {
  public:
    MotrInterface(std::unique_ptr<IMotrInterfaceImpl> impl = nullptr);

    ~MotrInterface();

    void initialize(const MotrConfig& config);

    void copy(const HsmObjectStoreRequest& request) const;

    void get(
        const HsmObjectStoreRequest& request,
        ostk::StorageObject& object,
        ostk::Stream* stream) const;

    void move(const HsmObjectStoreRequest& request) const;

    void put(const HsmObjectStoreRequest& request, ostk::Stream* stream) const;

    void remove(const HsmObjectStoreRequest& request) const;

  private:
    void validate_config(MotrConfig& config);

    void write_tier_info(
        const std::filesystem::path& path,
        const std::vector<MotrHsmTierInfo>& tier_info) const;

    std::unique_ptr<IMotrInterfaceImpl> m_impl;
};