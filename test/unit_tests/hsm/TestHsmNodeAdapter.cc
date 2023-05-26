#include <catch2/catch_all.hpp>

#include "HsmNodeAdapter.h"

#include <iostream>

TEST_CASE("Test HsmNodeAdapter", "[hsm]")
{
    hestia::HsmNode node;
    node.m_host_address = "127.0.0.1:8000";

    hestia::ObjectStoreBackend backend;
    backend.m_identifier = "my_backend";
    node.m_backends.push_back(backend);

    hestia::HsmNodeJsonAdapter adapter;
    std::string json;
    adapter.to_string(node, json);

    hestia::HsmNode reconstructed_node;
    adapter.from_string(json, reconstructed_node);

    REQUIRE(reconstructed_node.m_host_address == node.m_host_address);
    REQUIRE(reconstructed_node.m_backends.size() == 1);
    REQUIRE(
        reconstructed_node.m_backends[0].m_identifier == backend.m_identifier);
}