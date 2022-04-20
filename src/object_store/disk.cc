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
