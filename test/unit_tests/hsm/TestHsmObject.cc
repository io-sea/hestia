#include <catch2/catch_all.hpp>

#include "HsmObject.h"

TEST_CASE("Test HsmObject", "[hsm]")
{
    hestia::HsmObject object;
    object.set_size(1234);

    hestia::HsmObject copied_object = object;
    hestia::HsmObject copy_constructed_object(object);

    hestia::Dictionary object_dict;
    object.serialize(object_dict);

    hestia::Dictionary copied_dict;
    copied_object.serialize(copied_dict);

    hestia::Dictionary copy_constructed_dict;
    copy_constructed_object.serialize(copy_constructed_dict);

    REQUIRE(object_dict.to_string(true) == copied_dict.to_string(true));
    REQUIRE(
        copy_constructed_dict.to_string(true) == copied_dict.to_string(true));

    hestia::HsmObject deserialized_object;
    deserialized_object.deserialize(copied_dict);

    REQUIRE(deserialized_object.size() == object.size());
}
