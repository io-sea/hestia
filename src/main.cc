#include "hestia.h"
#include <iostream>

int main()
{
    struct hestia::hsm_uint oid(23905702934);
    struct hestia::hsm_obj obj;

    std::string write_data = "testing data 123";
    std::string read_data;
    read_data.resize(write_data.size());

    hestia::put(oid, &obj, false, write_data.data(), 0, write_data.size(), 0);

    hestia::get(oid, &obj, read_data.data(), 0, write_data.size(), 0, 0);

    std::cout << read_data << '\n';

    return 0;
}
