#include <fstream>

#include "disk.h"

int hestia::obj::Disk::put(
    const struct hsm_uint& oid,
    const void* buf,
    const std::size_t length,
    const std::uint8_t target_tier)
{
    std::ofstream file(
        std::to_string(target_tier) + '-' + std::to_string(oid.higher)
            + std::to_string(oid.lower) + ".data",
        std::ios_base::app);

    file.write(static_cast<const char*>(buf), length);

    return 0;
}

int hestia::obj::Disk::get(
    const struct hsm_uint& oid,
    void* buf,
    const std::size_t length,
    const std::uint8_t src_tier)
{
    std::ifstream file(
        std::to_string(src_tier) + '-' + std::to_string(oid.higher)
        + std::to_string(oid.lower) + ".data");

    file.read(static_cast<char*>(buf), length);

    return 0;
}
