#include "Datamover.h"

#include "Logger.h"

#include <filesystem>

int main(int argc, char**)
{
    if (argc < 4) {
        std::cerr << "Expects 3 Args: <subject> <uuid> <command>" << std::endl;
        return -1;
    }


    return 0;
}