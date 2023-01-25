#include <catch2/catch.hpp>
#include <hestia.h>
#include <string>

#include "common.h"

SCENARIO(
    "Copy interfaces correctly with the object store backend",
    "[object][store]")
{
    GIVEN("an object within the object store")
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

        const int dest_tier = 1;
        hestia::put(oid, false, data.data(), 0, data.size(), dest_tier);
        /*
                const auto json_attrs =
                    nlohmann::json::parse(hestia::get_attrs(oid, "tier"));
                const int dest_tier = json_attrs["tier"];
        */
        WHEN("the data is copied to a different tier")
        {
            // auto migration_call = nlohmann::json::parse(
            //    R"({"trigger_migration": {"operation":"copy",
            //    "src_tier":1,"tgt_tier":0}})");
            // migration_call["src_tier"] = dest_tier;
            //  add functionality to randomly choose the source and copy tiers
            // hestia::set_attrs(oid, migration_call.dump().data());
            hestia::set_attrs(
                oid,
                R"({"trigger_migration": {"operation":"copy", "src_tier":1,"tgt_tier":0}})");


            THEN(
                "The data retrieved from the original object matches the data retrieved from the copy")
            {

                std::string original_data;
                original_data.resize(data.size());
                hestia::get(
                    oid, &original_data[0], 0, original_data.size(), 0,
                    dest_tier);

                std::string copy_data;
                copy_data.resize(data.size());
                hestia::get(oid, &copy_data[0], 0, copy_data.size(), 0, 0);
                REQUIRE(original_data == copy_data);
            }
        }
        hestia::remove(oid);
    }
}
