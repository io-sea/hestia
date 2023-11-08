#include <catch2/catch_all.hpp>

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "ObjectStoreTestWrapper.h"

class S3ClientTestFixture : public ObjectStoreTestWrapper {
  public:
    S3ClientTestFixture() : ObjectStoreTestWrapper("mock_s3_plugin") {}
};

TEST_CASE_METHOD(S3ClientTestFixture, "S3 client put", "[.s3]")
{
    hestia::StorageObject obj("0000");
    const std::string content = "The quick brown fox jumps over the lazy dog.";

    put(obj, content);

    std::string returned_content;
    get(obj, returned_content, content.size());
    REQUIRE(returned_content == content);
}