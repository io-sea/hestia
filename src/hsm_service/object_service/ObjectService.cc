#include "ObjectService.h"

#include "HsmObjectAdapter.h"
#include "JsonUtils.h"
#include "KeyValueStoreClient.h"
#include "Logger.h"

namespace hestia {
ObjectService::ObjectService(
    KeyValueStoreClient* kv_store_client,
    HttpClient* http_client,
    std::unique_ptr<HsmObjectAdapter> object_adatper) :
    KeyValueCrudService<HsmObject>(kv_store_client),
    m_http_client(http_client),
    m_object_adapter(
        object_adatper ? std::move(object_adatper) : HsmObjectAdapter::create())
{
    (void)m_http_client;
}

ObjectService::~ObjectService() {}

void ObjectService::to_string(const HsmObject& item, std::string& output) const
{
    output = JsonUtils::to_json(item.object().m_metadata);
}

void ObjectService::from_string(
    const std::string& output, HsmObject& item) const
{
    JsonUtils::from_json(output, item.object().m_metadata);
}


}  // namespace hestia
