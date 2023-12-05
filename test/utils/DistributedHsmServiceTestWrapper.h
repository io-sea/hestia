#pragma once

#include "DistributedHsmService.h"
#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryKeyValueStoreClient.h"

#include "UserService.h"
#include "UserTokenGenerator.h"

class MockS3TokenGenerator : public hestia::UserTokenGenerator {
  public:
    std::string generate(const std::string& key = {}) const override
    {
        (void)key;
        return m_token;
    }
    std::string m_token{"wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY"};
};

class DistributedHsmServiceTestWrapper {
  public:
    void init(
        const std::string& user_name,
        const std::string& user_pass,
        std::size_t num_tiers);

    std::string get_user_token() const { return m_token_generator->m_token; }

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    hestia::InMemoryHsmObjectStoreClient m_obj_store_client;
    std::unique_ptr<hestia::UserService> m_user_service;
    std::unique_ptr<hestia::DistributedHsmService> m_dist_hsm_service;
    MockS3TokenGenerator* m_token_generator{nullptr};
};