#include <catch2/catch_all.hpp>

#include "HsmNode.h"
#include "StringAdapter.h"

TEST_CASE("Test HsmNode", "[hsm]")
{
    hestia::HsmNode node;
    node.set_host_address("127.0.0.1:8000");

    hestia::HsmObjectStoreClientBackend backend;
    backend.set_identifier("my_backend");
    node.add_backend(backend);

    hestia::HsmNode copied_node = node;
    hestia::HsmNode copy_constructed_node(node);

    hestia::Dictionary node_dict;
    node.serialize(node_dict);

    hestia::Dictionary copied_dict;
    copied_node.serialize(copied_dict);

    hestia::Dictionary copy_constructed_dict;
    copy_constructed_node.serialize(copy_constructed_dict);

    REQUIRE(node_dict.to_string() == copied_dict.to_string());
    REQUIRE(copy_constructed_dict.to_string() == copied_dict.to_string());

    hestia::HsmNode deserialized_node;
    deserialized_node.deserialize(copied_dict);

    REQUIRE(deserialized_node.host() == node.host());
    REQUIRE(deserialized_node.backends().size() == node.backends().size());
    REQUIRE(
        deserialized_node.backends()[0].get_identifier()
        == node.backends()[0].get_identifier());
}
