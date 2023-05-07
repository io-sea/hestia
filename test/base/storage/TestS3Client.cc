#include <catch2/catch_all.hpp>

#include "ObjectStoreTestWrapper.h"

class S3ClientTestFixture : public ObjectStoreTestWrapper {
  public:
    S3ClientTestFixture() : ObjectStoreTestWrapper("mock_s3_plugin") {}
};

TEST_CASE_METHOD(S3ClientTestFixture, "S3 client put", "[.][s3]")
{
    hestia::StorageObject obj("0000");
    const std::string content = "The quick brown fox jumps over the lazy dog.";

    hestia::ReadableBufferView read_buffer(content);
    hestia::Stream stream;
    put(obj, &stream);

    return;

    REQUIRE(stream.write(read_buffer).ok());
    REQUIRE(stream.reset().ok());

    std::vector<char> returned_buffer(content.length());
    hestia::WriteableBufferView write_buffer(returned_buffer);
    get(obj, &stream);

    REQUIRE(stream.read(write_buffer).ok());
    REQUIRE(stream.reset().ok());

    std::string recontstructed_content(
        returned_buffer.begin(), returned_buffer.end());
    REQUIRE(recontstructed_content == content);
}