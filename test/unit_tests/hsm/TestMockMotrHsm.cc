#include <catch2/catch_all.hpp>

#include "MockMotrHsm.h"

#include <iostream>

class MotrHsmTestFixture {
  public:
    MotrHsmTestFixture()
    {
        m_hsm.motr()->m0_client_init(&m_client, nullptr, true);
        hestia::mock::motr::Id realm_id{mo_uber_realm};
        m_hsm.motr()->m0_container_init(
            &m_container, nullptr, &realm_id, &m_client);
    }

    void set_up_pools(const std::vector<hestia::Uuid> pool_fids)
    {
        for (auto id : pool_fids) {
            hestia::Uuid pool_version_fid{id};
            m_client.add_pool(pool_version_fid);
        }
    }

    static constexpr int mo_uber_realm = 0;
    hestia::mock::motr::Hsm m_hsm;
    hestia::mock::motr::Client m_client;
    hestia::mock::motr::Container m_container;
};

static void* to_buffer(std::string& content)
{
    return reinterpret_cast<void*>(content.data());
}

static void* to_buffer(std::vector<char>& content)
{
    return reinterpret_cast<void*>(content.data());
}

TEST_CASE_METHOD(
    MotrHsmTestFixture,
    "Test Mock Motr HSM - Write and Read",
    "[mock-motr-hsm]")
{
    std::vector<hestia::Uuid> pool_fids = {{0}, {1}, {2}, {3}, {4}};
    set_up_pools(pool_fids);

    hestia::mock::motr::HsmOptions hsm_options;
    hsm_options.m_pool_fids = pool_fids;

    m_hsm.m0hsm_init(&m_client, &m_container.m_co_realm, &hsm_options);

    hestia::mock::motr::Id obj_id(0x1000000);

    hestia::mock::motr::Obj obj;
    int tier = 2;
    auto rc  = m_hsm.m0hsm_create(obj_id, &obj, tier, true);
    REQUIRE_FALSE(rc);

    std::string dump_target;
    rc = m_hsm.m0hsm_dump(dump_target, obj_id, false);
    REQUIRE_FALSE(rc);

    std::string content = "The quick brown fox jumps over the lazy dog.";

    auto buffer = to_buffer(content);
    rc          = m_hsm.m0hsm_pwrite(&obj, buffer, content.size(), 0);
    REQUIRE_FALSE(rc);

    dump_target.clear();
    rc = m_hsm.m0hsm_dump(dump_target, obj_id, false);
    REQUIRE_FALSE(rc);

    std::vector<char> sink(content.length());
    auto read_buffer = to_buffer(sink);
    rc               = m_hsm.m0hsm_read(obj_id, read_buffer, content.size(), 0);
    REQUIRE_FALSE(rc);

    std::string recontstructed_content(sink.begin(), sink.end());
    REQUIRE(recontstructed_content == content);
}

TEST_CASE_METHOD(
    MotrHsmTestFixture, "Test Mock Motr HSM - Copy", "[mock-motr-hsm]")
{
    std::vector<hestia::Uuid> pool_fids = {{0}, {1}, {2}, {3}, {4}};
    set_up_pools(pool_fids);

    hestia::mock::motr::HsmOptions hsm_options;
    hsm_options.m_pool_fids = pool_fids;

    m_hsm.m0hsm_init(&m_client, &m_container.m_co_realm, &hsm_options);

    hestia::mock::motr::Id obj_id(0x1000000);

    hestia::mock::motr::Obj obj;
    int tier = 2;
    auto rc  = m_hsm.m0hsm_create(obj_id, &obj, tier, true);
    REQUIRE_FALSE(rc);

    std::string dump_target;
    rc = m_hsm.m0hsm_dump(dump_target, obj_id, false);
    REQUIRE_FALSE(rc);

    std::string content = "The quick brown fox jumps over the lazy dog.";

    void* buffer = reinterpret_cast<void*>(content.data());
    rc           = m_hsm.m0hsm_pwrite(&obj, buffer, content.size(), 0);
    REQUIRE_FALSE(rc);

    dump_target.clear();
    rc = m_hsm.m0hsm_dump(dump_target, obj_id, false);
    REQUIRE_FALSE(rc);

    int src_tier    = 2;
    int tgt_tier    = 3;
    auto copy_flags = hestia::mock::motr::Hsm::hsm_cp_flags::HSM_MOVE;
    rc              = m_hsm.m0hsm_copy(
        obj_id, src_tier, tgt_tier, 0, content.size(), copy_flags);
    REQUIRE_FALSE(rc);

    dump_target.clear();
    rc = m_hsm.m0hsm_dump(dump_target, obj_id, false);
    REQUIRE_FALSE(rc);
}