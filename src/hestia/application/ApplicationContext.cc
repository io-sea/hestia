#include "ApplicationContext.h"

#include "DistributedHsmService.h"
#include "HsmObjectStoreClient.h"

#include "HttpClient.h"
#include "KeyValueStoreClient.h"

#include "UserService.h"

namespace hestia {

void ApplicationContext::clear()
{
    m_hsm_service.reset();
    m_kv_store_client.reset();
    m_object_store_client.reset();
    m_http_client.reset();
    m_user_service.reset();
}

ApplicationContext& ApplicationContext::get()
{
    static ApplicationContext instance;
    return instance;
}

UserService* ApplicationContext::get_user_service() const
{
    return m_user_service.get();
}

DistributedHsmService* ApplicationContext::get_hsm_service() const
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

HsmObjectStoreClient* ApplicationContext::get_object_store_client() const
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
    std::unique_ptr<HsmObjectStoreClient> object_store_client)
{
    m_object_store_client = std::move(object_store_client);
}

void ApplicationContext::set_hsm_service(DistributedHsmService::Ptr hsm_service)
{
    m_hsm_service = std::move(hsm_service);
}

void ApplicationContext::set_user_service(
    std::unique_ptr<UserService> user_service)
{
    m_user_service = std::move(user_service);
}
}  // namespace hestia