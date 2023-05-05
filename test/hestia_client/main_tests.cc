#include <catch2/catch_all.hpp>

#include "hestia.h"
#include <filesystem>
#include <iostream>
#include <string>

TEST_CASE("Hestia Original main test", "[hestia main]")
{
    std::filesystem::remove_all("kv_store");
    std::filesystem::remove_all("object_store");

    struct hestia::hsm_uint oid(23905702934);
    struct hestia::hsm_uint oid2(34016813045);
    struct hestia::hsm_uint set(12894691823);

    std::vector<hestia::hsm_uint> members = {oid, oid2};

    hestia::create_dataset(set, members);

    std::string write_data = "Buffer testing data 123";
    /*    std::ofstream testfile("testfile.txt");
        testfile.write(write_data.data(), write_data.size());
        testfile.close();

        int offset = 0;

        std::ifstream writefile("testfile.txt");
    */
    int offset = 0;
    if (hestia::put(oid, false, write_data.data(), offset, write_data.size(), 0)
        != 0) {
        std::cout << "put error!" << std::endl;
        REQUIRE(false);
    }
    //    writefile.close();


    if (hestia::put(
            oid2, false, write_data.data(), offset, write_data.size(), 0)
        != 0) {
        std::cout << "put error!" << std::endl;
        REQUIRE(false);
    }

    if (hestia::set_attrs(
            set,
            R"({"trigger_migration": {"operation":"copy","src_tier" : 0,
       "tgt_tier":1}})")
        != 0) {
        std::cout << "set_attrs error!" << std::endl;
        REQUIRE(false);
    }
    std::cout << "Copied dataset from tier 0 to tier 1\n";
    /*
        if (hestia::set_attrs(
                set,
                R"({"trigger_migration": {"operation":"release","src_tier" : 0,
           "tgt_tier":1}})")
            != 0) {
            std::cout << "set_attrs error!" << std::endl;
            exit(1);
        }

        if (hestia::set_attrs(
                set,
                R"({"trigger_migration": {"operation":"move","src_tier" : 1,
           "tgt_tier":0}})")
            != 0) {
            std::cout << "set_attrs error!" << std::endl;
            exit(1);
        }
        */
    std::string read_data;
    read_data.resize(write_data.size() - offset);
    std::string overwrite_data{"Overwriting data 123"};
    offset = 7;

    if (hestia::put(
            oid, true, overwrite_data.data(), offset, overwrite_data.size(), 0)
        != 0) {
        std::cout << "put error!" << std::endl;
        REQUIRE(false);
    }
    if (hestia::get(oid, &read_data[0], 7, read_data.size(), 0, 0) != 0) {
        std::cout << "get error!" << std::endl;
        REQUIRE(false);
    }

    std::cout << read_data << '\n';

    auto location  = hestia::locate(oid);
    auto location2 = hestia::locate(oid2);

    std::cout << "Object " << oid.higher << oid.lower << " is located on "
              << location.size() << " tiers\n";
    std::cout << "Object " << oid.higher << oid.lower
              << " Location tiers: " << std::endl;
    for (const auto& it : location) {
        std::cout << +it << std::endl;
    }
    std::cout << "Object " << oid2.higher << oid2.lower << " is located on "
              << location2.size() << " tiers\n";
    std::cout << "Object " << oid2.higher << oid2.lower
              << " Location tiers: " << std::endl;
    for (const auto& it : location2) {
        std::cout << +it << std::endl;
    }
    auto oids = hestia::list();

    const std::string test_key = "key";

    auto get_att = hestia::get_attrs(oid, test_key.data());

    std::cout << get_att << std::endl;

    std::cout << "Object list: \n";
    for (const auto& id : oids) {
        std::cout << id.higher << id.lower << std::endl;
    }

    auto tier_ids = hestia::list_tiers();
    for (auto i : tier_ids) {
        std::cout << unsigned(i) << std::endl;
    }

    std::vector<int> tids = {1};
    auto tiers_info       = hestia::get_tiers_info(tids);
    for (const auto& it : tiers_info) {
        std::cout << " " << +it.id << " " << it.store << " " << it.capacity
                  << " " << it.bandwith << std::endl;
    }

    auto member_list = hestia::get_dataset_members(set);

    std::cout << "Dataset members:\n";
    for (const auto& it : member_list) {
        std::cout << it.higher << '-' << it.lower << std::endl;
    }
    //        hestia::remove(set);
}