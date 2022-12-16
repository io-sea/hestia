#include "hestia.h"
#include <iostream>

int main()
{
    struct hestia::hsm_uint oid(23905702934);
    struct hestia::hsm_obj obj;

    std::string write_data = "testing data 123";
    if (hestia::put(
            oid, &obj, false, write_data.data(), 0, write_data.size(), 0)
        != 0) {
        std::cout << "put error!" << std::endl;
        exit(1);
    }

    if (hestia::set_attrs(
            oid,
            R"({"trigger_migration": {"operation":"copy","src_tier" : 0,
       "tgt_tier":1}})")
        != 0) {
        std::cout << "set_attrs error!" << std::endl;
        exit(1);
    }

    if (hestia::set_attrs(
            oid,
            R"({"trigger_migration": {"operation":"release","src_tier" : 0,
       "tgt_tier":1}})")
        != 0) {
        std::cout << "set_attrs error!" << std::endl;
        exit(1);
    }

    if (hestia::set_attrs(
            oid,
            R"({"trigger_migration": {"operation":"move","src_tier" : 1,
       "tgt_tier":0}})")
        != 0) {
        std::cout << "set_attrs error!" << std::endl;
        exit(1);
    }
    std::string read_data;
    read_data.resize(write_data.size());

    if (hestia::get(oid, &obj, &read_data[0], 0, write_data.size(), 0, 0)
        != 0) {
        std::cout << "get error!" << std::endl;
        exit(1);
    }

    std::cout << read_data << '\n';

    auto location = hestia::locate(oid);

    std::cout << "Object is located on " << location.size() << "tiers\n";
    std::cout << "Object Location tiers: " << std::endl;
    for (const auto& it : location) {
        std::cout << +it << std::endl;
    }
    auto oids = hestia::list();

    const std::string test_key = "key";

    auto get_att = hestia::get_attrs(oid, test_key.data());

    std::cout << get_att << std::endl;

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

    hestia::remove(oid);

    return 0;
}
