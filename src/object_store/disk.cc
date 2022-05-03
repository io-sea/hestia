#include <fstream>

#include "disk.h"

int hestia::obj::Disk::put(
    const struct hsm_uint& oid, const void* buf, std::size_t length)
{
    std::ofstream file(
        std::to_string(oid.higher) + std::to_string(oid.lower) + ".data");

    file.write(static_cast<const char*>(buf), length);

    return 0;
}

int hestia::obj::Disk::get(
    const struct hsm_uint& oid, void* buf, std::size_t length)
{
    std::ifstream file(
        std::to_string(oid.higher) + std::to_string(oid.lower) + ".data");

    file.read(static_cast<char*>(buf), length);

    return 0;
}
