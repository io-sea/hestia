#include <fstream>

#include "disk.h"

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
