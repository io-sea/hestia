#include "disk.h"
#include <filesystem>
#include <fstream>
#include <iostream>
int hestia::obj::Disk::put(
    const struct hsm_uint& oid,
    const void* buf,
    const std::size_t length,
    const std::uint8_t target_tier)
{
    std::ofstream file(get_filename_from_oid(oid, target_tier));

    file.write(static_cast<const char*>(buf), length);

    return 0;
}

int hestia::obj::Disk::get(
    const struct hsm_uint& oid,
    void* buf,
    const std::size_t length,
    const std::uint8_t src_tier)
{
    std::ifstream file(get_filename_from_oid(oid, src_tier));

    file.read(static_cast<char*>(buf), length);

    return 0;
}

int hestia::obj::Disk::remove(
    const struct hsm_uint& oid, const std::uint8_t tier)
{
    return static_cast<int>(
        std::filesystem::remove(get_filename_from_oid(oid, tier)));
}

int hestia::obj::Disk::copy(
    const struct hsm_uint& oid,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier)
{

    auto source(get_filename_from_oid(oid, src_tier));
    auto dest(get_filename_from_oid(oid, tgt_tier));
    if (!std::filesystem::exists(source)) {
        std::cout
            << "Error: Object to be copied does not exist on source tier\n";
        return -1;
    }
    std::filesystem::copy(source, dest);
    return 0;
}

int hestia::obj::Disk::move(
    const struct hsm_uint& oid,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier)
{

    auto source(get_filename_from_oid(oid, src_tier));
    auto dest(get_filename_from_oid(oid, tgt_tier));
    if (!std::filesystem::exists(source)) {
        std::cout
            << "Error: Object to be moved does not exist on source tier\n";
        return -1;
    }
    std::filesystem::rename(source, dest);
    return 0;
}
