#include <catch2/catch_all.hpp>

#include "MockMotr.h"
#include "MockMotrBackend.h"
#include "MockMotrHsm.h"

static char* to_buffer(std::string& content)
{
    return reinterpret_cast<char*>(content.data());
}

static char* to_buffer(std::vector<char>& content)
{
    return reinterpret_cast<char*>(content.data());
}

class MotrBackendTestFixture {
  public:
    MotrBackendTestFixture()
    {
        m_realm.m_client = &m_client;

        std::vector<unsigned> pool_fids{0, 1, 2, 3, 4};
        for (auto id : pool_fids) {
            ostk::Uuid pool_version_fid{id};
            m_client.add_pool(pool_version_fid);
        }
        m_backend.set_client(&m_client);
    }

    mock::motr::Client m_client;
    mock::motr::Realm m_realm;
    mock::motr::MotrBackend m_backend;
};

TEST_CASE_METHOD(
    MotrBackendTestFixture,
    "Test Mock Motr Backend - Standard Object",
    "[motr-backend]")
{
    return;
    auto layout = m_backend.allocate_layout(mock::motr::Layout::Type::DEFAULT);

    mock::motr::Obj obj;
    obj.m_layout_id = layout->m_id;
    obj.m_id        = mock::motr::Id(0);
    obj.m_realm     = &m_realm;

    std::string content("abcdefghijkl");
    auto buffer = to_buffer(content);

    mock::motr::IndexVec indices;
    indices.m_counts  = {3, 3, 3, 3};
    indices.m_indices = {0, 3, 6, 9};

    mock::motr::BufferVec data;
    data.m_counts  = {3, 3, 3, 3};
    data.m_buffers = {buffer, buffer + 3, buffer + 6, buffer + 9};

    auto rc = m_backend.write_object(obj, indices, data);
    REQUIRE_FALSE(rc);

    std::vector<char> sink(content.length());
    auto read_buffer = to_buffer(sink);

    mock::motr::BufferVec read_data;
    read_data.m_counts  = {3, 3, 3, 3};
    read_data.m_buffers = {
        read_buffer, read_buffer + 3, read_buffer + 6, read_buffer + 9};

    rc = m_backend.read_object(obj, indices, read_data);
    REQUIRE_FALSE(rc);

    std::string recontstructed_content(sink.begin(), sink.end());
    REQUIRE(recontstructed_content == content);
}

TEST_CASE_METHOD(
    MotrBackendTestFixture,
    "Test Mock Motr Backend - Composite Object",
    "[motr-backend]")
{
    return;
    auto composite_layout =
        m_backend.allocate_layout(mock::motr::Layout::Type::COMPOSITE);

    mock::motr::Obj obj;
    obj.m_layout_id = composite_layout->m_id;
    obj.m_id        = mock::motr::Id(0);
    obj.m_realm     = &m_realm;

    auto l0_layout =
        m_backend.allocate_layout(mock::motr::Layout::Type::DEFAULT);
    mock::motr::Obj sub_obj;
    sub_obj.m_id        = mock::motr::Id(1);
    sub_obj.m_layout_id = l0_layout->m_id;
    sub_obj.m_realm     = &m_realm;
    sub_obj.m_pool      = m_client.get_pool(ostk::Uuid(1))->id();

    m_backend.add_object(&sub_obj);

    mock::motr::Motr::m0_composite_layer_add(composite_layout, &sub_obj, 0);
    auto layer = m_backend.get_layer(sub_obj.m_id);

    ostk::Extent write_ext;
    write_ext.m_offset = 0;
    write_ext.m_length = 12;
    layer->add_extent(write_ext, true, true);

    ostk::Extent read_ext;
    read_ext.m_offset = 0;
    read_ext.m_length = 12;
    layer->add_extent(read_ext, false, true);

    std::string content("abcdefghijkl");
    auto buffer = to_buffer(content);

    mock::motr::IndexVec indices;
    indices.m_counts  = {3, 3, 3, 3};
    indices.m_indices = {0, 3, 6, 9};

    mock::motr::BufferVec data;
    data.m_counts  = {3, 3, 3, 3};
    data.m_buffers = {buffer, buffer + 3, buffer + 6, buffer + 9};

    auto rc = m_backend.write_object(obj, indices, data);
    REQUIRE_FALSE(rc);

    std::vector<char> sink(content.length());
    auto read_buffer = to_buffer(sink);

    mock::motr::BufferVec read_data;
    read_data.m_counts  = {3, 3, 3, 3};
    read_data.m_buffers = {
        read_buffer, read_buffer + 3, read_buffer + 6, read_buffer + 9};

    rc = m_backend.read_object(obj, indices, read_data);
    REQUIRE_FALSE(rc);

    std::string recontstructed_content(sink.begin(), sink.end());
    REQUIRE(recontstructed_content == content);
}