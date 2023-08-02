#include <catch2/catch_all.hpp>

#include "StorageTier.h"
#include "StringAdapter.h"

#include <iostream>

TEST_CASE("Test Storage Tier", "[hsm]")
{
    hestia::StorageTier tier;
    tier.set_capacity(1234);
    tier.set_bandwidth(5678);

    auto copied_tier = tier;
    hestia::StorageTier copy_constructed_tier(tier);

    REQUIRE(copied_tier.get_bandwidth() == tier.get_bandwidth());
    REQUIRE(copied_tier.get_capacity() == tier.get_capacity());
    REQUIRE(copied_tier.get_backends().size() == tier.get_backends().size());

    hestia::Dictionary tier_dict;
    tier.serialize(tier_dict);

    hestia::Dictionary copied_tier_dict;
    copied_tier.serialize(copied_tier_dict);

    hestia::Dictionary copy_constructed_tier_dict;
    copy_constructed_tier.serialize(copy_constructed_tier_dict);

    REQUIRE(tier_dict.to_string(true) == copied_tier_dict.to_string(true));
    REQUIRE(
        copy_constructed_tier_dict.to_string(true)
        == copied_tier_dict.to_string(true));

    hestia::StorageTier deserialized_tier;
    deserialized_tier.deserialize(copied_tier_dict);

    REQUIRE(deserialized_tier.get_bandwidth() == tier.get_bandwidth());
    REQUIRE(deserialized_tier.get_capacity() == tier.get_capacity());
    REQUIRE(
        deserialized_tier.get_backends().size() == tier.get_backends().size());

    hestia::StorageTier tier_with_id(3);
    REQUIRE(tier_with_id.id_uint() == 3);
    REQUIRE(tier_with_id.name() == "3");
}