#pragma once

#include "DistributedHsmService.h"
#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"
#include "UserService.h"

class DistributedHsmServiceTestWrapper {
  public:
    DistributedHsmServiceTestWrapper();

    void add_tiers(std::size_t num_tiers);

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    hestia::InMemoryHsmObjectStoreClient m_obj_store_client;
    std::unique_ptr<hestia::UserService> m_user_service;
    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;
};