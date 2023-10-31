#include <catch2/catch_all.hpp>

#include "HsmNode.h"

TEST_CASE("Test HsmNode", "[hsm]")
{
    hestia::HsmNode node;
    node.set_host_address("127.0.0.1:8000");

    hestia::HsmNode copied_node = node;
    hestia::HsmNode copy_constructed_node(node);

    hestia::Dictionary node_dict;
    node.serialize(node_dict);

    hestia::Dictionary copied_dict;
    copied_node.serialize(copied_dict);

    hestia::Dictionary copy_constructed_dict;
    copy_constructed_node.serialize(copy_constructed_dict);

    hestia::Dictionary::FormatSpec dict_format;
    dict_format.m_map_format.m_sort_keys = true;

    REQUIRE(
        node_dict.to_string(dict_format) == copied_dict.to_string(dict_format));
    REQUIRE(
        copy_constructed_dict.to_string(dict_format)
        == copied_dict.to_string(dict_format));

    hestia::HsmNode deserialized_node;
    deserialized_node.deserialize(copied_dict);

    REQUIRE(deserialized_node.host() == node.host());
    REQUIRE(deserialized_node.backends().size() == node.backends().size());
}
