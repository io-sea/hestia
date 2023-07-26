#include "hestia/hestia.h"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    if (argc == 2) {
        hestia_initialize(argv[1], nullptr, nullptr);
    }
    else {
        hestia_initialize(nullptr, nullptr, nullptr);
    }

    /*
    std::vector<char> buffer(HESTIA_UUID_SIZE, 0);
    int rc = hestia_object_create(buffer.data());
    std::string id(buffer.begin(), buffer.end());

    std::string content("The quick brown fox jumps over the lazy dog");
    hestia_object_put(id.c_str(), content.data(), 0, content.length(), 0);

    hestia_object_move(id.c_str(), 0, 1);

    std::vector<char> data_buffer(content.length(), 0);
    hestia_object_get(id.c_str(), buffer.data(), 0, content.length(), 1);

    std::string returned_content(data_buffer.begin(), data_buffer.end());

    if (content == returned_content) {
        std::cout << "Sanity check OK" << std::endl;
        hestia_finish();
        return 0;
    }
    else {
        std::cerr << "Sanity check failed" << std::endl;
        hestia_finish();
        return -1;
    }
    */
   hestia_finish();
   return 0;
}

