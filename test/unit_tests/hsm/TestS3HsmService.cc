#include <catch2/catch_all.hpp>

#include "DistributedHsmServiceTestWrapper.h"
#include "S3Service.h"

#include "TestUtils.h"

#include <iostream>

#ifdef __HESTIA_DUMMY__
class HsmS3ServiceTestFixture {
  public:
    void init(const std::string& test_name)
    {
        m_dist_hsm_service = std::make_unique<DistributedHsmServiceTestWrapper>(
            __FILE__, test_name);
        m_dist_hsm_service->add_tiers(5);

        m_s3_service = std::make_unique<hestia::S3Service>(
            m_dist_hsm_service->m_dist_hsm_service.get(),
            &m_dist_hsm_service->m_kv_store_client);
    }

    void check_ok(hestia::S3Service::Status status) { REQUIRE(status.ok()); }

    void check_exists(const hestia::Dataset& dataset, bool should_exist = true)
    {
        const auto [status, exists] =
            m_s3_service->get_hsm_serive()->get_hsm_service()
                REQUIRE(status.ok());
        REQUIRE(exists == should_exist);
    }

    void check_exists(const hestia::S3Object& object, bool should_exist = true)
    {
        const auto [status, exists] = m_s3_service->exists(object);
        REQUIRE(status.ok());
        REQUIRE(exists == should_exist);
    }

    std::unique_ptr<DistributedHsmServiceTestWrapper> m_dist_hsm_service;
    std::unique_ptr<hestia::HsmS3Service> m_s3_service;
};

TEST_CASE_METHOD(
    HsmS3ServiceTestFixture, "HSM S3 Service test", "[hsm-service]")
{
    init("TestHsmS3Service");

    hestia::S3Container container("mybucket");
    container.m_user_metadata.set_item("mykey", "myval");

    check_ok(m_s3_service->put(container));

    check_exists(container);

    std::vector<hestia::S3Container> containers;
    check_ok(m_s3_service->list(containers));

    REQUIRE(containers.size() == 1);

    hestia::S3Container fetched_container("mybucket");
    check_ok(m_s3_service->get(fetched_container));
    REQUIRE(fetched_container == container);

    hestia::S3Object object("0000");
    object.m_container = "mybucket";
    object.m_user_metadata.set_item("myobjkey", "myobjval");

    check_ok(m_s3_service->put(container, object));

    check_exists(object);

    std::vector<hestia::S3Object> objects;
    check_ok(m_s3_service->list(container, objects));
    REQUIRE(objects.size() == 1);

    hestia::S3Object fetched_object("0000");
    check_ok(m_s3_service->get(fetched_object));

    REQUIRE(fetched_object == object);
}
#endif
