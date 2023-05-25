#include <catch2/catch_all.hpp>

#include "FifoStreamSink.h"
#include "FifoStreamSource.h"
#include "FileStreamSink.h"
#include "FileStreamSource.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "Stream.h"

#include "TestUtils.h"

TEST_CASE("Test In Memory Stream Flush", "[stream]")
{
    hestia::Stream stream;

    const std::string data = "The quick brown fox jumps over the lazy dog.";
    stream.set_source(hestia::InMemoryStreamSource::create(data));

    std::vector<char> result_buffer(data.size());
    stream.set_sink(hestia::InMemoryStreamSink::create(result_buffer));

    REQUIRE(stream.flush().ok());

    std::string result(result_buffer.begin(), result_buffer.end());
    REQUIRE(result == data);
}

TEST_CASE("Test File Stream IO", "[stream]")
{
    hestia::Stream stream;

    const std::string data = "The quick brown fox jumps over the lazy dog.";
    stream.set_source(hestia::InMemoryStreamSource::create(data));

    const auto test_dir =
        TestUtils::get_test_output_dir(__FILE__) / "FileStreamIO";
    std::filesystem::remove_all(test_dir);
    const auto test_file = test_dir / "test.dat";

    stream.set_sink(hestia::FileStreamSink::create(test_file));
    REQUIRE(stream.flush().ok());

    stream.set_source(hestia::FileStreamSource::create(test_file));

    std::vector<char> result_buffer(data.size());
    stream.set_sink(hestia::InMemoryStreamSink::create(result_buffer));
    REQUIRE(stream.flush().ok());

    std::string result(result_buffer.begin(), result_buffer.end());
    REQUIRE(result == data);
}

TEST_CASE("Test Fifo Stream Write", "[stream]")
{
    hestia::Stream stream;

    const std::string data = "The quick brown fox jumps over the lazy dog.";

    auto fifo_sink = hestia::FifoStreamSink::create();
    auto read_fd   = fifo_sink->get_read_descriptor();

    stream.set_sink(std::move(fifo_sink));
    REQUIRE(stream.write(hestia::ReadableBufferView(data)).ok());

    std::vector<char> buffer(data.length());
    const auto& [status, size] =
        hestia::SystemUtils::do_read(read_fd, &buffer[0], data.length());
    REQUIRE(status.ok());

    std::string result(buffer.begin(), buffer.end());
    REQUIRE(result == data);
}

TEST_CASE("Test Fifo Stream Read", "[stream]")
{
    hestia::Stream stream;

    const std::string data = "The quick brown fox jumps over the lazy dog.";

    auto fifo_source = hestia::FifoStreamSource::create();
    auto write_fd    = fifo_source->get_write_descriptor();
    stream.set_source(std::move(fifo_source));

    std::vector<char> buffer(data.begin(), data.end());
    const auto& [status, size] =
        hestia::SystemUtils::do_write(write_fd, &buffer[0], data.length());
    REQUIRE(status.ok());

    std::vector<char> result_buffer(data.length());
    hestia::WriteableBufferView buffer_view(result_buffer);
    REQUIRE(stream.read(buffer_view).ok());

    std::string result(result_buffer.begin(), result_buffer.end());
    REQUIRE(result == data);
}