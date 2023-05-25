#include <catch2/catch_all.hpp>

#include "InMemoryObjectStoreClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "S3ContainerModel.h"
#include "S3ObjectModel.h"
#include "S3Service.h"

class TestS3ServiceFixture {
  public:
    TestS3ServiceFixture()
    {
        m_client = hestia::InMemoryObjectStoreClient::create();

        hestia::S3ServiceConfig service_config;
        service_config.m_object_store_client = m_client.get();
        m_s3_service = std::make_unique<hestia::S3Service>(service_config);
    }

    void check_ok(hestia::S3Service::Status status) { REQUIRE(status.ok()); }

    void check_exists(const hestia::S3Object& object, bool should_exist = true)
    {
        const auto [status, exists] = m_s3_service->exists(object);
        REQUIRE(status.ok());
        REQUIRE(exists == should_exist);
    }

    void check_exists(
        const hestia::S3Container& container, bool should_exist = true)
    {
        const auto [status, exists] = m_s3_service->exists(container);
        REQUIRE(status.ok());
        REQUIRE(exists == should_exist);
    }

    std::unique_ptr<hestia::InMemoryObjectStoreClient> m_client;
    std::unique_ptr<hestia::S3Service> m_s3_service;
};

TEST_CASE_METHOD(TestS3ServiceFixture, "Test s3 service", "[s3]")
{
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

    check_ok(m_s3_service->remove(container));

    check_exists(container, false);

    check_ok(m_s3_service->put(container));

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

    check_ok(m_s3_service->remove(object));

    check_exists(object, false);
}