#pragma once

#include "ObjectStoreClient.h"
#include "SerializeableWithFields.h"

#include <memory>

namespace hestia {

class HttpClient;

class HttpObjectStoreClientConfig : public SerializeableWithFields {
  public:
    HttpObjectStoreClientConfig() :
        SerializeableWithFields("http_object_store_config")
    {
        register_scalar_field(&m_endpoint);
        register_scalar_field(&m_endpoint_suffix);
    }

    StringField m_endpoint{"endpoint"};
    StringField m_endpoint_suffix{"endpoint_suffix"};
};

class HttpObjectStoreClient : public ObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<HttpObjectStoreClient>;

    HttpObjectStoreClient(HttpClient* http_client);

    virtual ~HttpObjectStoreClient() = default;

    static Ptr create(HttpClient* http_client);

    static std::string get_registry_identifier();

    void initialize(
        const std::string& cache_path, const Dictionary& config) override;

    void do_initialize(
        const std::string& cache_path,
        const HttpObjectStoreClientConfig& config);

  private:
    bool exists(const StorageObject& object) const override;

    void put(const StorageObject& object, const Extent& extent, Stream* stream)
        const override;

    void get(StorageObject& object, const Extent& extent, Stream* stream)
        const override;

    void remove(const StorageObject& obj) const override;

    void list(const KeyValuePair& query, std::vector<StorageObject>& found)
        const override;

    HttpObjectStoreClientConfig m_config;
    HttpClient* m_http_client;
};
}  // namespace hestia