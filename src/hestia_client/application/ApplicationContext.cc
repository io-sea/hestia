#include "ApplicationContext.h"

#include "HsmService.h"

ApplicationContext& ApplicationContext::get()
{
    static ApplicationContext instance;
    return instance;
}

HsmService* ApplicationContext::getHsmService() const
{
    return mHsmService.get();
}

void ApplicationContext::setHsmService(HsmService::Ptr hsmService)
{
    mHsmService = std::move(hsmService);
}