#include "ApplicationContext.h"

#include "HsmService.h"
#include "MultiBackendHsmObjectStoreClient.h"

#include "HttpClient.h"
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

HttpClient* ApplicationContext::get_http_client() const
{
    return m_http_client.get();
}

MultiBackendHsmObjectStoreClient* ApplicationContext::get_object_store_client()
    const
{
    return m_object_store_client.get();
}

void ApplicationContext::set_kv_store_client(
    std::unique_ptr<KeyValueStoreClient> kv_store_client)
{
    m_kv_store_client = std::move(kv_store_client);
}

void ApplicationContext::set_http_client(
    std::unique_ptr<HttpClient> http_client)
{
    m_http_client = std::move(http_client);
}

void ApplicationContext::set_object_store_client(
    std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store_client)
{
    m_object_store_client = std::move(object_store_client);
}

void ApplicationContext::set_hsm_service(HsmService::Ptr hsm_service)
{
    m_hsm_service = std::move(hsm_service);
}
}  // namespace hestia