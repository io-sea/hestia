#include <catch2/catch_all.hpp>

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "ObjectStoreTestWrapper.h"

class S3ClientTestFixture : public ObjectStoreTestWrapper {
  public:
    S3ClientTestFixture() : ObjectStoreTestWrapper("mock_s3_plugin") {}
};

TEST_CASE_METHOD(S3ClientTestFixture, "S3 client put", "[s3]")
{
    hestia::StorageObject obj("0000");
    const std::string content = "The quick brown fox jumps over the lazy dog.";

    hestia::ReadableBufferView read_buffer(content);
    auto source = hestia::InMemoryStreamSource::create(read_buffer);

    hestia::Stream stream;
    stream.set_source(std::move(source));

    put(obj, &stream);
    REQUIRE(stream.reset().ok());

    std::vector<char> returned_buffer(content.length());
    hestia::WriteableBufferView write_buffer(returned_buffer);

    auto sink = hestia::InMemoryStreamSink::create(write_buffer);
    stream.set_sink(std::move(sink));

    get(obj, &stream);
    REQUIRE(stream.reset().ok());

    std::string recontstructed_content(
        returned_buffer.begin(), returned_buffer.end());
    REQUIRE(recontstructed_content == content);
}