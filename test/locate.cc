#include "common.h"
#include "tiers.h"
#include <catch2/catch.hpp>
#include <hestia.h>
#include <string>

SCENARIO("Locate interfaces correctly with the kv store backend", "[kv][store]")
{
    GIVEN("an object in hte object store")
    {
        auto data_vec = GENERATE(
            chunk(max_data_size, take(max_data_size, random(' ', 'z'))));

        std::string data(data_vec.begin(), data_vec.end());

        auto hsm_uint_parts = GENERATE(chunk(
            2, take(
                   2, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));
        struct hestia::hsm_uint oid(hsm_uint_parts[0], hsm_uint_parts[1]);
        struct hestia::hsm_obj obj;

        // randomly generate a tier for the object to be sent to
        auto num_tiers = static_cast<int>((hestia::tiers).size());
        auto dest_tier = GENERATE_REF(take(
            1, filter(  // NOLINT
                   [&num_tiers](auto var) {
                       return var < static_cast<int>(num_tiers);
                   },
                   random(0, 10))));

        hestia::put(oid, &obj, false, data.data(), 0, data.size(), dest_tier);

        WHEN("locate is used to retrieve the tier for the object")
        {
            auto located_tier = hestia::locate(oid);
            THEN("the retrieved tier matches the original destination tier")
            {
                REQUIRE(located_tier == dest_tier);
            }
        }
        hestia::remove(oid);
    }
}
