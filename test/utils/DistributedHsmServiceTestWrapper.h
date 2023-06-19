#pragma once

#include "DistributedHsmService.h"
#include "FileHsmObjectStoreClient.h"
#include "FileKeyValueStoreClient.h"

class DistributedHsmServiceTestWrapper {
  public:
    DistributedHsmServiceTestWrapper(
        const std::string& test_file, const std::string& test_name);

    ~DistributedHsmServiceTestWrapper();

    void add_tiers(std::size_t num_tiers);

    void get_store_path() const;

    std::string m_store_path;
    hestia::FileKeyValueStoreClient m_kv_store_client;
    hestia::FileHsmObjectStoreClient m_obj_store_client;
    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;
};