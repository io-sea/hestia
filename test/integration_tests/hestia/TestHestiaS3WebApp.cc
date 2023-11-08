#include <catch2/catch_all.hpp>

#include "BasicHttpServer.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "FileHsmObjectStoreClient.h"
#include "FileKeyValueStoreClient.h"

#include "HestiaS3WebApp.h"
#include "HsmService.h"
#include "UserService.h"

#include "DistributedHsmServiceTestWrapper.h"
#include "ObjectStoreTestWrapper.h"

#include "TestUtils.h"

#include <iostream>

class S3ClientTestFixture : public ObjectStoreTestWrapper {
  public:
    S3ClientTestFixture() : ObjectStoreTestWrapper("s3_plugin")
    {
        m_fixture = std::make_unique<DistributedHsmServiceTestWrapper>();
        m_fixture->init("AKIAIOSFODNN7EXAMPLE", "my_admin_password", 5);

        hestia::HestiaS3WebAppConfig web_app_config;
        m_web_app = std::make_unique<hestia::HestiaS3WebApp>(
            web_app_config, m_fixture->m_dist_hsm_service.get(),
            m_fixture->m_user_service.get());

        hestia::Server::Config server_config;
        m_server = std::make_unique<hestia::BasicHttpServer>(
            server_config, m_web_app.get());

        m_server->initialize();
        m_server->start();
        m_server->wait_until_bound();

        m_client->initialize("0", {}, {});
    }

    std::unique_ptr<DistributedHsmServiceTestWrapper> m_fixture;

    std::unique_ptr<hestia::HestiaS3WebApp> m_web_app;
    std::unique_ptr<hestia::BasicHttpServer> m_server;

    std::string m_base_url = "127.0.0.1:8000/";
};

TEST_CASE_METHOD(S3ClientTestFixture, "Test Hestia S3 Web App", "[.s3]")
{
    auto user_id = "AKIAIOSFODNN7EXAMPLE";
    auto user_key =
        m_fixture->m_user_service->get_current_user_context().m_token;

    const auto id = "0000";
    hestia::StorageObject obj(id);
    obj.set_metadata("my_key", "my_value");
    obj.set_metadata("hestia-bucket_name", "my_bucket");
    obj.set_metadata("hestia-user_id", user_id);
    obj.set_metadata("hestia-user_token", user_key);

    const std::string content = "The quick brown fox jumps over the lazy dog.";
    put(obj, content);

    std::string recontstructed_content;
    get(obj, recontstructed_content, content.size());
    REQUIRE(recontstructed_content == content);
}