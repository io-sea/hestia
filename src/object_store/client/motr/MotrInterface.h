#pragma once

#include "IMotrInterfaceImpl.h"

#include <ostk/Uuid.h>

#include <memory>
#include <string>
#include <filesystem>

struct m0_obj;
struct m0_uint128;

class MotrObjImpl;

class MotrObject
{
public:
    MotrObject(const ostk::Uuid& oid);

    ~MotrObject();

    m0_uint128 getMotrFid() const;

    m0_obj& getMotrObj() const;

    static m0_uint128 toMotrId(const ostk::Uuid& id);

private:
    ostk::Uuid mId;
    std::unique_ptr<MotrObjImpl> mHandleImpl;
};

class MotrInterface
{
public:
    MotrInterface(std::unique_ptr<IMotrInterfaceImpl> impl = nullptr);

    ~MotrInterface();

    void initialize(const MotrConfig& config);

    void copy(const HsmObjectStoreRequest& request) const;

    void get(const HsmObjectStoreRequest& request, ostk::StorageObject& object, ostk::Stream* stream) const;

    void move(const HsmObjectStoreRequest& request) const;

    void put(const HsmObjectStoreRequest& request, ostk::Stream* stream) const;

    void remove(const HsmObjectStoreRequest& request) const;

private:
    void validateConfig(MotrConfig& config);

    void writeTierInfo(const std::filesystem::path& path, const std::vector<MotrHsmTierInfo>& tierInfo) const;

    std::unique_ptr<IMotrInterfaceImpl> mImpl;
};