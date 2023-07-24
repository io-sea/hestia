#include "HttpObjectStoreClient.h"

#include "HttpClient.h"
#include "ProjectConfig.h"

#include "Logger.h"

namespace hestia {
HttpObjectStoreClient::HttpObjectStoreClient(HttpClient* http_client) :
    m_http_client(std::move(http_client))
{
}

HttpObjectStoreClient::Ptr HttpObjectStoreClient::create(
    HttpClient* http_client)
{
    return std::make_unique<HttpObjectStoreClient>(http_client);
}

std::string HttpObjectStoreClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name()
           + "::HttpObjectStoreClient";
}

void HttpObjectStoreClient::initialize(
    const std::string& cache_path, const Dictionary& config_data)
{
    HttpObjectStoreClientConfig config;
    config.deserialize(config_data);
    do_initialize(cache_path, config);
}

void HttpObjectStoreClient::do_initialize(
    const std::string&, const HttpObjectStoreClientConfig& config)
{
    m_config = config;
}

void HttpObjectStoreClient::put(
    const StorageObject& object, const Extent& extent, Stream* stream) const
{
    (void)object;
    (void)extent;
    (void)stream;
}

void HttpObjectStoreClient::get(
    StorageObject& object, const Extent& extent, Stream* stream) const
{
    const auto path = m_config.m_endpoint.get_value() + "/" + object.id();
    auto http_response =
        m_http_client->make_request({path, HttpRequest::Method::GET});
    if (http_response->error()) {
        throw ObjectStoreException(
            {ObjectStoreErrorCode::ERROR,
             "Failed to contact remote object host: "
                 + http_response->to_string()});
    }
    (void)extent;
    (void)stream;
}

void HttpObjectStoreClient::remove(const StorageObject& object) const
{
    (void)object;
}

void HttpObjectStoreClient::list(
    const KeyValuePair& query, std::vector<StorageObject>& found) const
{
    (void)query;
    (void)found;
}

bool HttpObjectStoreClient::exists(const StorageObject& object) const
{
    (void)object;
    return false;
}
}  // namespace hestia