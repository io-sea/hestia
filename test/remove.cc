#include <catch2/catch.hpp>
#include <filesystem>
#include <hestia.h>
#include <string>

#include "common.h"
#include "kv_store/disk.h"
#include "object_store/disk.h"

SCENARIO(
    "Remove erases the object from the KV store and the object store",
    "[kv][object][store]")
{
    GIVEN("an object existing in the KV store and the object store")
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

        const auto tier = 0;
        hestia::put(oid, &obj, false, data.data(), 0, data.size(), tier);

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

            /* TODO: more general way of testing for object presence */
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
