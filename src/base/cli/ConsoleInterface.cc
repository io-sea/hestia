#include "ConsoleInterface.h"

#include <iostream>

namespace hestia {
void ConsoleInterface::console_write(const std::string& output) const
{
    std::cout << output << std::endl;
}

void ConsoleInterface::console_write(
    const std::vector<std::string>& output) const
{
    for (const auto& item : output) {
        console_write(item);
    }
}

void ConsoleInterface::console_write_error(const std::string& output) const
{
    std::cerr << output << std::endl;
}

void ConsoleInterface::console_read(std::string& buffer) const
{
    char c{0};
    while (std::cin.get(c)) {
        buffer += c;
    }
}
}  // namespace hestia