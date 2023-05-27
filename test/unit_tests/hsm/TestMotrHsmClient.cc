#include <catch2/catch_all.hpp>

#include "HsmObjectStoreTestWrapper.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "TestContext.h"

#include "hestia.h"

class MockMotrTestFixture : public HsmObjectStoreTestWrapper {
  public:
    MockMotrTestFixture() : HsmObjectStoreTestWrapper("mock_motr_plugin")
    {
        hestia::Metadata config;
        config.set_item("tier_info", "identifier=1;identifier=2;identifier=3");

        m_client->initialize(config);
    }
};

TEST_CASE_METHOD(MockMotrTestFixture, "Motr client write and read", "[motr]")
{
    hestia::StorageObject obj("0000");

    std::string content = "The quick brown fox jumps over the lazy dog";
    obj.m_size          = content.size();

    hestia::Stream stream;
    auto source = hestia::InMemoryStreamSource::create(
        hestia::ReadableBufferView(content));
    stream.set_source(std::move(source));

    put(obj, &stream, 1);
    REQUIRE(stream.flush().ok());

    std::vector<char> returned_buffer(content.length());
    hestia::WriteableBufferView write_buffer(returned_buffer);
    auto sink = hestia::InMemoryStreamSink::create(write_buffer);
    stream.set_sink(std::move(sink));

    return;

    get(obj, &stream, 1);
    REQUIRE(stream.flush().ok());

    std::string returned_content =
        std::string(returned_buffer.begin(), returned_buffer.end());
    REQUIRE(returned_content == content);
}
