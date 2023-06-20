#pragma once

#include "CrudService.h"
#include "Dataset.h"

#include <memory>

namespace hestia {

class KeyValueStoreClient;
class HttpClient;

struct DatasetServiceConfig {
    std::string m_endpoint;
    std::string m_global_prefix;
    std::string m_item_prefix{"dataset"};
};

class DatasetService : public CrudService<Dataset> {
  public:
    using Ptr = std::unique_ptr<DatasetService>;

    DatasetService(
        const DatasetServiceConfig& config,
        std::unique_ptr<CrudClient<Dataset>> client);

    static Ptr create(
        const DatasetServiceConfig& config, KeyValueStoreClient* client);

    static Ptr create(const DatasetServiceConfig& config, HttpClient* client);

    virtual ~DatasetService() = default;

  private:
    DatasetServiceConfig m_config;
};
}  // namespace hestia