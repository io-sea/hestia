#include "MotrInterface.h"

#ifdef HAS_MOTR_IMPL
#include "MotrInterfaceImpl.h"
#endif

#include <fstream>

MotrInterface::MotrInterface(std::unique_ptr<IMotrInterfaceImpl> impl) :
    mImpl(std::move(impl))
{
#ifdef HAS_MOTR_IMPL
    if (!mImpl) {
        mImpl = std::make_unique<MotrInterfaceImpl>();
    }
#endif
}

MotrInterface::~MotrInterface() {}

void MotrInterface::initialize(const MotrConfig& config)
{
    MotrConfig working_config = config;
    validateConfig(working_config);
    mImpl->initialize(working_config);
}

void MotrInterface::validateConfig(MotrConfig& config)
{
    auto working_path = config.mHsmConfigPath;
    if (working_path.empty()) {
        working_path = std::filesystem::current_path() / "config";
        writeTierInfo(working_path, config.mTierInfo);
    }
    config.mHsmConfigPath = working_path;
}

void MotrInterface::copy(const HsmObjectStoreRequest& request) const
{
    mImpl->copy(request);
}

void MotrInterface::get(
    const HsmObjectStoreRequest& request,
    ostk::StorageObject& object,
    ostk::Stream* stream) const
{
    mImpl->get(request, object, stream);
}

void MotrInterface::move(const HsmObjectStoreRequest& request) const
{
    mImpl->move(request);
}

void MotrInterface::put(
    const HsmObjectStoreRequest& request, ostk::Stream* stream) const
{
    mImpl->put(request, stream);
}

void MotrInterface::remove(const HsmObjectStoreRequest& request) const
{
    mImpl->remove(request);
}

void MotrInterface::writeTierInfo(
    const std::filesystem::path& path,
    const std::vector<MotrHsmTierInfo>& tierInfo) const
{
    std::ofstream f_out;
    f_out.open(path);
    for (const auto& tier : tierInfo) {
        f_out << tier.mName << " = <" << tier.mIdentifier << "> \n";
    }
    f_out.close();
}