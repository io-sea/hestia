#include "ApplicationContext.h"

#include "HsmService.h"

ApplicationContext& ApplicationContext::get()
{
    static ApplicationContext instance;
    return instance;
}

HsmService* ApplicationContext::get_hsm_service() const
{
    return m_hsm_service.get();
}

void ApplicationContext::set_hsm_service(HsmService::Ptr hsm_service)
{
    m_hsm_service = std::move(hsmService);
}