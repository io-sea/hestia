#include "ConsoleInterface.h"

#include <iostream>

namespace hestia {
void ConsoleInterface::console_write(const std::string& output) const
{
    std::cout << output << std::endl;
}

bool ConsoleInterface::console_read(std::string& line) const
{
    return bool(std::getline(std::cin, line));
}
}  // namespace hestia