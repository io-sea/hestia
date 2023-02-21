#include <catch2/catch.hpp>
#include <hestia.h>
#include <string>

#include "common.h"

SCENARIO(
    "Move interfaces correctly with the object store backend",
    "[object][store]")
{
    GIVEN("an object and a dataset with two objects within the object store")
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

        // object 1
        auto data_vec1 = GENERATE(
            chunk(max_data_size, take(max_data_size, random(' ', 'z'))));

        std::string data1(data_vec1.begin(), data_vec1.end());

        auto hsm_uint_parts1 = GENERATE(chunk(
            2, take(
                   2, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));
        struct hestia::hsm_uint oid1(hsm_uint_parts1[0], hsm_uint_parts1[1]);

        // object 2
        auto data_vec2 = GENERATE(
            chunk(max_data_size, take(max_data_size, random(' ', 'z'))));

        std::string data2(data_vec2.begin(), data_vec2.end());

        auto hsm_uint_parts2 = GENERATE(chunk(
            2, take(
                   2, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));
        struct hestia::hsm_uint oid2(hsm_uint_parts2[0], hsm_uint_parts2[1]);

        // dataset object
        auto hsm_uint_parts3 = GENERATE(chunk(
            2, take(
                   2, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));
        struct hestia::hsm_uint set(hsm_uint_parts3[0], hsm_uint_parts3[1]);

        std::vector<hestia::hsm_uint> members = {oid1, oid2};
        hestia::create_dataset(set, members);


        const int dest_tier = 0;
        hestia::put(oid, false, data.data(), 0, data.size(), dest_tier);
        hestia::put(oid1, false, data1.data(), 0, data1.size(), dest_tier);
        hestia::put(oid2, false, data2.data(), 0, data2.size(), dest_tier);
        /*
                const auto json_attrs =
                    nlohmann::json::parse(hestia::get_attrs(oid, "tier"));
                const int dest_tier = json_attrs["tier"];
        */
        WHEN("the data is moved to a different tier")
        {
            std::string original_data;
            original_data.resize(data.size());
            hestia::get(
                oid, &original_data[0], 0, original_data.size(), 0, dest_tier);

            hestia::set_attrs(
                oid,
                R"({"trigger_migration": {"operation":"move", "src_tier":0,"tgt_tier":1}})");


            THEN(
                "The data retrieved from the original object above matches the data retrieved from the move")
            {

                /*              const auto json_attrs =
                                  nlohmann::json::parse(hestia::get_attrs(oid,
                   "tier")); const int tgt_tier = json_attrs["tier"];
                  */
                std::string move_data;
                move_data.resize(data.size());
                hestia::get(oid, &move_data[0], 0, move_data.size(), 0, 1);
                REQUIRE(original_data == move_data);
            }
        }
        WHEN("the dataset is moved to a different tier")
        {
            std::string original_data1;
            original_data1.resize(data1.size());
            hestia::get(
                oid1, &original_data1[0], 0, original_data1.size(), 0,
                dest_tier);

            std::string original_data2;
            original_data2.resize(data2.size());
            hestia::get(
                oid2, &original_data2[0], 0, original_data2.size(), 0,
                dest_tier);

            hestia::set_attrs(
                set,
                R"({"trigger_migration": {"operation":"move", "src_tier":0,"tgt_tier":1}})");


            THEN(
                "The data retrieved from the original object above matches the data retrieved from the move")
            {

                /*              const auto json_attrs =
                                  nlohmann::json::parse(hestia::get_attrs(oid,
                   "tier")); const int tgt_tier = json_attrs["tier"];
                  */
                std::string move_data1;
                move_data1.resize(data1.size());
                hestia::get(oid1, &move_data1[0], 0, move_data1.size(), 0, 1);
                REQUIRE(original_data1 == move_data1);

                std::string move_data2;
                move_data2.resize(data2.size());
                hestia::get(oid2, &move_data2[0], 0, move_data2.size(), 0, 1);
                REQUIRE(original_data2 == move_data2);
            }
        }
        hestia::remove(oid);
        hestia::remove(set);
    }
}
