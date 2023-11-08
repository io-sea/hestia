#pragma once

#include "S3Bucket.h"
#include "S3Client.h"
#include "S3Config.h"
#include "S3Object.h"

#include "S3BucketAdapter.h"
#include "S3ObjectAdapter.h"
#include "StorageObject.h"

#include "ObjectStoreClient.h"

#include <memory>

namespace hestia {

class S3ObjectStoreClient : public ObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<S3ObjectStoreClient>;

    S3ObjectStoreClient(S3Client* s3_client);

    virtual ~S3ObjectStoreClient() = default;

    static Ptr create(S3Client* s3_client);

    static Ptr create(
        const std::string& id, const S3Config& config, S3Client* s3_client);

    static std::string get_registry_identifier();

    void initialize(
        const std::string& id,
        const std::string& cache_path,
        const Dictionary& config) override;

    void do_initialize(
        const std::string& id,
        const std::string& cache_path,
        const S3Config& config);

  private:
    bool exists(const StorageObject& object) const override;

    void put(
        const ObjectStoreRequest& request,
        completionFunc completion_func,
        Stream* stream                     = nullptr,
        Stream::progressFunc progress_func = nullptr) const override;

    void get(
        const ObjectStoreRequest& request,
        completionFunc completion_func,
        Stream* stream                     = nullptr,
        Stream::progressFunc progress_func = nullptr) const override;

    void remove(const StorageObject& obj) const override;

    void list(const KeyValuePair& query, std::vector<StorageObject>& found)
        const override;

    S3Config m_config;
    S3Client* m_s3_client{nullptr};
    std::unique_ptr<S3BucketAdapter> m_container_adapter;
    std::unique_ptr<S3ObjectAdapter> m_object_adapter;
};
}  // namespace hestia