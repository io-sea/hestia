#include "ApplicationContext.h"

#include "HsmObjectStoreClient.h"
#include "HsmService.h"
#include "KeyValueStoreClient.h"

namespace hestia {

void ApplicationContext::clear()
{
    m_hsm_service.reset();
    m_kv_store_client.reset();
    m_object_store_client.reset();
}

ApplicationContext& ApplicationContext::get()
{
    static ApplicationContext instance;
    return instance;
}

HsmService* ApplicationContext::get_hsm_service() const
{
    return m_hsm_service.get();
}

KeyValueStoreClient* ApplicationContext::get_kv_store_client() const
{
    return m_kv_store_client.get();
}

void ApplicationContext::set_kv_store_client(
    std::unique_ptr<KeyValueStoreClient> kv_store_client)
{
    m_kv_store_client = std::move(kv_store_client);
}

void ApplicationContext::set_object_store_client(
    std::unique_ptr<HsmObjectStoreClient> object_store_client)
{
    m_object_store_client = std::move(object_store_client);
}

void ApplicationContext::set_hsm_service(HsmService::Ptr hsm_service)
{
    m_hsm_service = std::move(hsm_service);
}
}  // namespace hestia