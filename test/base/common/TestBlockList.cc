#include <catch2/catch_all.hpp>

#include "BlockList.h"
#include <iostream>

TEST_CASE("Test BlockList Operations - Chunk Write", "[blocklist]")
{
    std::string content = "The quick brown fox jumps over the lazy dog";

    hestia::BlockList block_list;

    std::size_t chunk_size = 10;
    std::vector<char> data_chars(content.begin(), content.end());
    std::size_t cursor = 0;
    while (cursor < content.size()) {
        auto chunk_end = cursor + chunk_size;
        if (chunk_end >= content.size()) {
            chunk_end = content.size();
        }
        if (chunk_end == cursor) {
            break;
        }

        std::vector<char> chunk(
            data_chars.begin() + cursor, data_chars.begin() + chunk_end);
        hestia::ReadableBufferView read_buffer(chunk);
        block_list.write(
            hestia::Extent(cursor, chunk_end - cursor), read_buffer);
        cursor = chunk_end;
    }

    std::vector<char> ret_buffer(content.size());
    hestia::WriteableBufferView write_buffer(ret_buffer);
    block_list.read({}, write_buffer);

    std::string returned_content =
        std::string(ret_buffer.begin(), ret_buffer.end());
    REQUIRE(returned_content == content);
}

TEST_CASE("Test BlockList Operations - Chunk Read", "[blocklist]")
{
    std::string content = "The quick brown fox jumps over the lazy dog";

    hestia::BlockList block_list;
    hestia::ReadableBufferView read_buffer(content);
    block_list.write({}, read_buffer);

    std::size_t chunk_size = 10;
    std::vector<char> data_chars(content.size());
    std::size_t cursor = 0;
    while (cursor < content.size()) {
        auto chunk_end = cursor + chunk_size;
        if (chunk_end >= content.size()) {
            chunk_end = content.size();
        }
        if (chunk_end == cursor) {
            break;
        }

        hestia::WriteableBufferView write_buffer(
            &data_chars[0] + cursor, chunk_end - cursor);
        block_list.read(
            hestia::Extent(cursor, chunk_end - cursor), write_buffer);
        cursor = chunk_end;
    }

    std::vector<char> ret_buffer(content.size());
    hestia::WriteableBufferView write_buffer(ret_buffer);
    block_list.read({}, write_buffer);

    std::string returned_content =
        std::string(ret_buffer.begin(), ret_buffer.end());
    REQUIRE(returned_content == content);
}

TEST_CASE("Test Block List Read Operations - Multiple Write", "[blocklist]")
{
    hestia::BlockList block_list;

    std::string block0{"abc"};
    std::string block1{"def"};
    std::string block2{"ghi"};
    std::string block3{"jkl"};
    const std::string id{"0000"};

    block_list.write({0, block0.length()}, hestia::ReadableBufferView(block0));

    SECTION("Read back from overlapping block overwrite")
    {
        auto block1_2 = block1 + block2;

        block_list.write(
            {block0.length(), block1_2.length()},
            hestia::ReadableBufferView(block1_2));

        std::cerr << block_list.dump() << std::endl;

        block_list.write(
            {1, block3.length()}, hestia::ReadableBufferView(block3));

        std::cerr << block_list.dump() << std::endl;

        std::vector<char> sink(block0.length() + block1_2.length());
        hestia::WriteableBufferView write_buffer(sink);
        const auto& [ok, bytes_read] =
            block_list.read({0, sink.size()}, write_buffer);
        REQUIRE(ok);

        std::string recontstructed_content(sink.begin(), sink.end());
        REQUIRE(recontstructed_content == "ajklefghi");
    }
}