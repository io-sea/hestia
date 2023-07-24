#include <catch2/catch_all.hpp>

#include "BlockStore.h"
#include "TestUtils.h"

#include <filesystem>
#include <iostream>

TEST_CASE("Test Block Store Read Operations - Single Write", "[block_store]")
{
    hestia::BlockStore store;

    std::string content{"0123456789"};
    const std::string id{"0000"};
    store.write(id, {0, content.length()}, hestia::ReadableBufferView(content));

    SECTION("Read back full extents")
    {
        std::vector<char> sink(content.length());
        hestia::WriteableBufferView write_buffer(sink);
        auto rc = store.read(id, {0, sink.size()}, write_buffer);
        REQUIRE(rc.is_ok());

        std::string recontstructed_content(sink.begin(), sink.end());
        REQUIRE(recontstructed_content == content);
    }

    SECTION("Read back partial extents")
    {
        std::size_t part_offset = 3;
        std::size_t part_length = 4;
        std::vector<char> sink(part_length);
        hestia::WriteableBufferView write_buffer(sink);
        auto rc = store.read(id, {part_offset, sink.size()}, write_buffer);
        REQUIRE(rc.is_ok());

        std::string recontstructed_content(sink.begin(), sink.end());
        REQUIRE(
            recontstructed_content == content.substr(part_offset, part_length));
    }

    SECTION("Try to read non-existing fid")
    {
        std::vector<char> sink(content.length());
        hestia::WriteableBufferView write_buffer(sink);
        auto rc = store.read("1111", {0, sink.size()}, write_buffer);
        REQUIRE(
            rc.m_status
            == hestia::BlockStore::ReturnCode::Status::ID_NOT_FOUND);
    }

    SECTION("Try to read fully out of bounds extents")
    {
        std::vector<char> sink(content.length());
        hestia::WriteableBufferView write_buffer(sink);
        auto rc =
            store.read(id, {2 * content.length(), sink.size()}, write_buffer);
        REQUIRE(
            rc.m_status
            == hestia::BlockStore::ReturnCode::Status::EXTENT_NOT_FOUND);
    }

    SECTION("Try to read partially out of bounds extents")
    {
        std::vector<char> sink(content.length());
        hestia::WriteableBufferView write_buffer(sink);
        std::size_t offset = int(0.5 * content.length());
        auto rc = store.read(id, {offset, sink.size()}, write_buffer);
        REQUIRE(
            rc.m_status
            == hestia::BlockStore::ReturnCode::Status::EXTENT_NOT_FOUND);
    }
}

TEST_CASE("Test Block Store Read Operations - Multiple Write", "[block_store]")
{
    hestia::BlockStore store;

    std::string block0{"abc"};
    std::string block1{"def"};
    std::string block2{"ghi"};
    std::string block3{"jkl"};
    const std::string id{"0000"};

    store.write(id, {0, block0.length()}, hestia::ReadableBufferView(block0));

    SECTION("Read back from consecutive block writes")
    {
        auto rc = store.write(
            id, {block0.length(), block1.length()},
            hestia::ReadableBufferView(block1));
        REQUIRE(rc.is_ok());

        std::vector<char> sink(block0.length() + block1.length());
        hestia::WriteableBufferView write_buffer(sink);
        rc = store.read(id, {0, sink.size()}, write_buffer);
        REQUIRE(rc.is_ok());

        std::string recontstructed_content(sink.begin(), sink.end());
        REQUIRE(recontstructed_content == (block0 + block1));
    }

    SECTION("Read back from non-consecutive block writes")
    {
        auto rc = store.write(
            id, {block0.length() + 2, block1.length()},
            hestia::ReadableBufferView(block1));
        REQUIRE(rc.is_ok());

        std::vector<char> sink(block0.length() + block1.length());
        hestia::WriteableBufferView write_buffer(sink);
        rc = store.read(id, {0, sink.size()}, write_buffer);
        REQUIRE(
            rc.m_status
            == hestia::BlockStore::ReturnCode::Status::EXTENT_NOT_FOUND);
    }

    SECTION("Read back from block overwrite")
    {
        auto rc = store.write(
            id, {0, block1.length()}, hestia::ReadableBufferView(block1));
        REQUIRE(rc.is_ok());

        std::vector<char> sink(block1.length());
        hestia::WriteableBufferView write_buffer(sink);
        rc = store.read(id, {0, sink.size()}, write_buffer);
        REQUIRE(rc.is_ok());

        std::string recontstructed_content(sink.begin(), sink.end());
        REQUIRE(recontstructed_content == block1);
    }

    SECTION("Read back from inclusive block overwrite")
    {
        auto block1_2 = block1 + block2;

        auto rc = store.write(
            id, {block0.length(), block1_2.length()},
            hestia::ReadableBufferView(block1_2));
        REQUIRE(rc.is_ok());

        rc = store.write(
            id, {4, block3.length()}, hestia::ReadableBufferView(block3));
        REQUIRE(rc.is_ok());

        std::vector<char> sink(6);
        hestia::WriteableBufferView write_buffer(sink);
        rc = store.read(id, {2, sink.size()}, write_buffer);
        REQUIRE(rc.is_ok());

        std::string recontstructed_content(sink.begin(), sink.end());
        REQUIRE(recontstructed_content == "cdjklh");
    }
}

TEST_CASE("Test Block Store IO", "[block_store]")
{
    hestia::BlockStore store;
    auto work_dir = TestUtils::get_test_output_dir(__FILE__);

    std::string content{"0123456789"};
    store.write(
        "0000", {0, content.length()}, hestia::ReadableBufferView(content));
    store.write(
        "0000", {content.length(), content.length()},
        hestia::ReadableBufferView(content));
    store.write(
        "1111", {0, content.length()}, hestia::ReadableBufferView(content));

    // store.dump(work_dir / "TestBlockStoreIO");
}