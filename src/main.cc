#include "hestia.h"
#include <iostream>

int main()
{
    struct hestia::hsm_uint oid(23905702934);
    struct hestia::hsm_obj obj;

    void* data{0};
    char test    = 'R';
    int test_len = 1;
    data         = &test;
    void* recv   = new char[test_len];

    hestia::put(oid, &obj, false, data, 0, test_len * sizeof(char), 0);

    hestia::get(oid, &obj, recv, 0, test_len * sizeof(char), 0, 0);

    std::cout.write(static_cast<char*>(recv), test_len);
    return 0;
}
