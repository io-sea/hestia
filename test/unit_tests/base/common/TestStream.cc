#include <catch2/catch_all.hpp>

#include "FifoStreamSink.h"
#include "FifoStreamSource.h"
#include "FileStreamSink.h"
#include "FileStreamSource.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "Stream.h"
#include "SystemUtils.h"

#include "TestUtils.h"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

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

    stream.set_source(hestia::InMemoryStreamSource::create(data));
    REQUIRE(stream.supports_source_seek());

    int offset = 10;
    stream.seek_source_to(offset);

    std::vector<char> offset_result_buffer(data.size() - offset);
    stream.set_sink(hestia::InMemoryStreamSink::create(offset_result_buffer));
    REQUIRE(stream.flush().ok());

    std::string offset_result(
        offset_result_buffer.begin(), offset_result_buffer.end());
    std::string offset_data = data.substr(offset);
    REQUIRE(offset_result == offset_data);
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

    auto file_source = hestia::FileStreamSource::create(test_file);
    REQUIRE(!file_source->empty());


    stream.set_source(hestia::FileStreamSource::create(test_file));
    REQUIRE(stream.supports_source_seek());

    std::vector<char> result_buffer(data.size());
    stream.set_sink(hestia::InMemoryStreamSink::create(result_buffer));
    REQUIRE(stream.flush().ok());

    std::string result(result_buffer.begin(), result_buffer.end());
    REQUIRE(result == data);

    stream.set_source(hestia::FileStreamSource::create(test_file));

    REQUIRE(stream.supports_source_seek());

    std::size_t offset = 10;
    stream.seek_source_to(offset);

    std::vector<char> offset_result_buffer(data.size() - offset);
    stream.set_sink(hestia::InMemoryStreamSink::create(offset_result_buffer));
    REQUIRE(stream.flush().ok());

    std::string offset_result(
        offset_result_buffer.begin(), offset_result_buffer.end());
    std::string offset_data = data.substr(offset);
    REQUIRE(offset_result == offset_data);
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

    REQUIRE(!stream.supports_source_seek());

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

TEST_CASE("Test ProgressFunc", "[stream]")
{
    hestia::Stream stream;
    const std::string data = "12345678901234567890";
    // generate a random string of length 100, to test the progress func
    int count                             = 0;
    std::function<void(int)> progressfunc = [&count, &stream](int x) {
        std::cout << x << " bytes transferred" << std::endl;
        REQUIRE(static_cast<int>(stream.get_num_transferred()) == x);
        ++count;
    };
    std::size_t interval = 4;

    stream.set_source(hestia::InMemoryStreamSource::create(data));

    stream.set_progress_func(interval, progressfunc);

    std::vector<char> result_buffer(data.size());
    stream.set_sink(hestia::InMemoryStreamSink::create(result_buffer));

    std::size_t flush_rate = 2;

    REQUIRE(stream.get_num_transferred() == 0);

    REQUIRE(stream.flush(flush_rate).ok());

    REQUIRE(count == 5);

    REQUIRE(!stream.flush().message().empty());
}

TEST_CASE("Test POSIX file stream IO using file descriptors", "[stream]")
{
    hestia::Stream stream;

    const std::string data = "The quick brown fox jumps over the lazy dog.";
    stream.set_source(hestia::InMemoryStreamSource::create(data));

    const auto test_dir =
        TestUtils::get_test_output_dir(__FILE__) / "POSIXFileStreamIO";
    std::filesystem::remove_all(test_dir);
    std::filesystem::create_directories(test_dir);
    const auto test_file = test_dir / "test.dat";

    int fd;

    // does not work unless the file already exists

    fd = open(test_file.c_str(), O_WRONLY | O_CREAT | O_EXCL, S_IRWXU);

    if (fd < 0) {
        std::cout << "Error: " << std::strerror(errno) << std::endl;
    }

    stream.set_sink(hestia::FileStreamSink::create(fd, data.length()));
    REQUIRE(stream.flush().ok());

    int file_descriptors[1] = {fd};

    REQUIRE(hestia::SystemUtils::close_files(file_descriptors, 1).ok());

    close(fd);

    fd = open(test_file.c_str(), O_RDONLY);

    stream.set_source(hestia::FileStreamSource::create(fd, data.length()));

    std::vector<char> result_buffer(data.size());
    stream.set_sink(hestia::InMemoryStreamSink::create(result_buffer));
    // std::cout<<stream.flush().message()<<std::endl;
    REQUIRE(stream.flush().ok());

    std::string result(result_buffer.begin(), result_buffer.end());
    REQUIRE(result == data);
    close(fd);
}