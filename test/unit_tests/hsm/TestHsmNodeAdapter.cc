#include <catch2/catch_all.hpp>

#include "HsmNodeAdapter.h"
#include "JsonUtils.h"

#include <iostream>

TEST_CASE("Test HsmNodeAdapter", "[hsm]")
{
    hestia::HsmNode node;
    node.m_host_address = "127.0.0.1:8000";

    hestia::HsmObjectStoreClientBackend backend;
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

TEST_CASE("Test HsmNodeAdapter - Node List", "[hsm]")
{
    hestia::HsmNode node0;
    node0.m_host_address = "127.0.0.1:8000";
    node0.m_id           = "1234";

    hestia::HsmNode node1;
    node1.m_host_address = "127.0.0.1:8080";
    node1.m_id           = "5678";

    std::vector<hestia::HsmNode> nodes;
    nodes.push_back(node0);
    nodes.push_back(node1);

    hestia::HsmNodeJsonAdapter adapter;
    std::string json;
    adapter.to_string(nodes, json);

    auto dict = hestia::JsonUtils::from_json(json);
    REQUIRE(dict->get_type() == hestia::Dictionary::Type::SEQUENCE);
    REQUIRE(dict->get_sequence().size() == 2);
}
