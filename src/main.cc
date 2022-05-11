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

    if (hestia::set_attrs(oid, R"({"key": "value"})") != 0) {
        std::cout << "set_attrs error!" << std::endl;
        exit(1);
    }

    std::string read_data;
    read_data.resize(write_data.size());

    if (hestia::get(oid, &obj, read_data.data(), 0, write_data.size(), 0, 0)
        != 0) {
        std::cout << "get error!" << std::endl;
        exit(1);
    }

    std::cout << read_data << '\n';

    auto oids = hestia::list();

    for (const auto& id : oids) {
        std::cout << id.higher << id.lower << std::endl;
    }

    // hestia::remove(oid);

    return 0;
}
