#include <catch2/catch_all.hpp>

#include "StorageTier.h"

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

    hestia::Dictionary::FormatSpec dict_format;
    dict_format.m_map_format.m_sort_keys = true;

    REQUIRE(
        tier_dict.to_string(dict_format)
        == copied_tier_dict.to_string(dict_format));
    REQUIRE(
        copy_constructed_tier_dict.to_string(dict_format)
        == copied_tier_dict.to_string(dict_format));

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

TEST_CASE("Sequencefield with tiers", "[hsm], [common]")
{
    hestia::StorageTier tier0;
    tier0.set_capacity(1234);
    tier0.set_bandwidth(5678);

    hestia::StorageTier tier1;
    tier1.set_capacity(9012);
    tier1.set_bandwidth(3456);

    hestia::StorageTier tier2;
    tier2.set_capacity(7890);
    tier2.set_bandwidth(1357);

    hestia::SequenceField<std::vector<hestia::StorageTier>> testfield(
        "testField");

    testfield.get_container_as_writeable() = {tier0, tier1, tier2};

    hestia::Dictionary tiers_dict;

    testfield.serialize(tiers_dict);

    hestia::SequenceField<std::vector<hestia::StorageTier>> result_field(
        "ResultField");

    result_field.deserialize(tiers_dict);

    REQUIRE(testfield.container().size() == result_field.container().size());

    auto i2 = result_field.container().begin();
    for (auto i1 = testfield.container().begin();
         i1 != testfield.container().end(); i1++, i2++) {
        REQUIRE(i1->get_bandwidth() == i2->get_bandwidth());
        REQUIRE(i1->get_capacity() == i2->get_capacity());
        REQUIRE(i1->get_backends().size() == i2->get_backends().size());
    }
}