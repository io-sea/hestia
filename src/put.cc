#include "../test/data_placement_engine/eejit.h"
#include "../test/kv_store/disk.h"
#include "../test/object_store/disk.h"
#include "hestia.h"
#include "tiers.h"
#include <chrono>
#include <cstdio>

void hestia::create_dataset(
    const struct hsm_uint oid, std::vector<hsm_uint>& members)
{
    create_object(oid);

    nlohmann::json meta_data{};
    meta_data["dataset"] = true;

    for (auto& it : members) {
        meta_data["members"].push_back(
            std::to_string(it.higher) + "-" + std::to_string(it.lower));
    }

    kv::Disk kv_store;
    kv_store.put_meta_data(oid, meta_data);
}

void hestia::create_object(const struct hsm_uint& oid)
{
    auto time_since_epoch =
        std::chrono::system_clock::now().time_since_epoch().count();

    nlohmann::json meta_data{};
    meta_data["creation_time"] = time_since_epoch;
    meta_data["last_modified"] = time_since_epoch;

    meta_data["dataset"] = false;

    auto num_tiers = hestia::list_tiers().size();

    for (auto i = 0; i < static_cast<int>(num_tiers); i++) {
        meta_data["tiers"][i] = false;
    }

    kv::Disk kv_store;
    kv_store.put_meta_data(oid, meta_data);
}

int hestia::put(
    const struct hsm_uint oid,
    const bool is_overwrite,
    const void* buf,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier)
{
    kv::Disk kv_store;

    /*
     * if the object already exists and we are not attempting to overwrite or if
     * the object doesn't exist and we are attempting to overwrite we should
     * stop here and return an error (TODO: appropriate error code)
     */
    if (kv_store.object_exists(oid) != is_overwrite) {
        return 1;
    }

    nlohmann::json meta_data;
    if (!is_overwrite) {
        if (offset != 0) {
            return 1;
        }
        create_object(oid);
    }
    else {
        meta_data["last_modified"] =
            std::chrono::system_clock::now().time_since_epoch().count();
    }

    dpe::Eejit dpe(tiers);
    const auto chosen_tier = dpe.choose_tier(length, target_tier);

    /* add object tier to metadata */
    meta_data["tiers"][chosen_tier] = true;

    /* interface with kv store */

    kv_store.put_meta_data(oid, meta_data);

    /* send data to backend */
    obj::Disk object_store;
    object_store.put(
        oid, static_cast<const char*>(buf), offset, length, chosen_tier);

    return 0;
}
/*
int hestia::put(
    const struct hsm_uint oid,
    const bool is_overwrite,
    std::ifstream& infile,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier)
{
    // interface with kv store
    kv::Disk kv_store;

    //
    // if the object already exists and we are not attempting to overwrite or if
    // the object doesn't exist and we are attempting to overwrite we should
    // stop here and return an error (TODO: appropriate error code)

    if (kv_store.object_exists(oid) != is_overwrite) {
        return 1;
    }
    nlohmann::json meta_data;
    if (!is_overwrite) {
        create_object(oid);
    }
    else {
        meta_data["last_modified"] =
            std::chrono::system_clock::now().time_since_epoch().count();
    }

    dpe::Eejit dpe(tiers);
    const auto chosen_tier = dpe.choose_tier(length, target_tier);

    // add object tier to metadata
    meta_data["tiers"][chosen_tier] = true;


    kv_store.put_meta_data(oid, meta_data);

    // send data to backend
    obj::Disk object_store;
    std::string buf;
    buf.resize(length);
    if (!infile.is_open()) {
        return 2;
    }

    infile.seekg(offset, infile.beg);

    infile.read(&buf[0], length);

    object_store.put(oid, buf.data(), length, chosen_tier);

    return 0;
}*/
