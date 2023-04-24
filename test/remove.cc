#include <catch2/catch_all.hpp>
#include <filesystem>
#include <hestia.h>
#include <string>

#include "common.h"
#include "hsm_object_store/disk.h"
#include "key_value_store/disk.h"

SCENARIO(
    "Remove erases the object from the KV store and the object store",
    "[kv][object][store]")
{
    GIVEN("an object in the KV store and the object store")
    {
        auto data_vec = GENERATE(
            chunk(max_data_size, take(max_data_size, random(32, 122))));

        std::string data(data_vec.begin(), data_vec.end());

        auto hsm_uint_parts = GENERATE(chunk(
            2, take(
                   2, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));
        struct hestia::hsm_uint oid(hsm_uint_parts[0], hsm_uint_parts[1]);


        const auto tier = 0;
        hestia::put(oid, false, data.data(), 0, data.size(), tier);

        /*
                const auto json_attrs =
                    nlohmann::json::parse(hestia::get_attrs(oid, "tier"));
                const auto tier = json_attrs["tier"];
        */
        std::string md_filename = hestia::kv::Disk().get_path() + '/'
                                  + std::to_string(oid.higher) + '-'
                                  + std::to_string(oid.lower) + ".meta";
        std::string data_filename = hestia::obj::Disk().get_tier_path(tier)
                                    + '/' + std::to_string(oid.higher)
                                    + std::to_string(oid.lower) + ".data";

        /* TODO: more general way of testing for object presence */
        /* ensure object exists in KV store and object store */
        REQUIRE(std::filesystem::exists(md_filename));
        REQUIRE(std::filesystem::exists(data_filename));


        WHEN("the object is removed")
        {
            hestia::remove(oid);

            // TODO: more general way of testing for object presence
            THEN("the object does not exist in the KV store")
            {
                REQUIRE(!std::filesystem::exists(md_filename));
            }
            THEN("the object does not exist in the object store")
            {
                REQUIRE(!std::filesystem::exists(data_filename));
            }
        }
    }
}
SCENARIO(
    "Remove erases the dataset from the KV store and the object store",
    "[kv][object][store]")
{
    GIVEN(
        "a dataset with two objects existing in the KV store and the object store")
    {

        // object 1
        auto data_vec1 = GENERATE(
            chunk(max_data_size, take(max_data_size, random(32, 122))));

        std::string data1(data_vec1.begin(), data_vec1.end());

        auto hsm_uint_parts1 = GENERATE(chunk(
            2, take(
                   2, random(
                          std::numeric_limits<std::uint64_t>::min(),
                          std::numeric_limits<std::uint64_t>::max()))));
        struct hestia::hsm_uint oid1(hsm_uint_parts1[0], hsm_uint_parts1[1]);

        // object 2
        auto data_vec2 = GENERATE(
            chunk(max_data_size, take(max_data_size, random(32, 122))));

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

        const auto tier = 0;
        hestia::put(oid1, false, data1.data(), 0, data1.size(), tier);
        hestia::put(oid2, false, data2.data(), 0, data2.size(), tier);

        /*
                const auto json_attrs =
                    nlohmann::json::parse(hestia::get_attrs(oid, "tier"));
                const auto tier = json_attrs["tier"];
        */

        // Files for object 1
        std::string md_filename1 = hestia::kv::Disk().get_path() + '/'
                                   + std::to_string(oid1.higher) + '-'
                                   + std::to_string(oid1.lower) + ".meta";
        std::string data_filename1 = hestia::obj::Disk().get_tier_path(tier)
                                     + '/' + std::to_string(oid1.higher)
                                     + std::to_string(oid1.lower) + ".data";

        REQUIRE(std::filesystem::exists(md_filename1));
        REQUIRE(std::filesystem::exists(data_filename1));

        // Files for object 2
        std::string md_filename2 = hestia::kv::Disk().get_path() + '/'
                                   + std::to_string(oid2.higher) + '-'
                                   + std::to_string(oid2.lower) + ".meta";
        std::string data_filename2 = hestia::obj::Disk().get_tier_path(tier)
                                     + '/' + std::to_string(oid2.higher)
                                     + std::to_string(oid2.lower) + ".data";

        REQUIRE(std::filesystem::exists(md_filename2));
        REQUIRE(std::filesystem::exists(data_filename2));

        // Files for dataset
        std::string md_filenameset = hestia::kv::Disk().get_path() + '/'
                                     + std::to_string(set.higher) + '-'
                                     + std::to_string(set.lower) + ".meta";

        REQUIRE(std::filesystem::exists(md_filenameset));


        WHEN("the object and dataset is removed")
        {
            hestia::remove(set);

            // TODO: more general way of testing for object presence
            THEN("the object does not exist in the KV store")
            {
                REQUIRE(!std::filesystem::exists(md_filename1));
                REQUIRE(!std::filesystem::exists(md_filename2));
                REQUIRE(!std::filesystem::exists(md_filenameset));
            }
            THEN("the object does not exist in the object store")
            {
                REQUIRE(!std::filesystem::exists(data_filename1));
                REQUIRE(!std::filesystem::exists(data_filename2));
            }
        }
    }
}
