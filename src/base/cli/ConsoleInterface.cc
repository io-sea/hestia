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

void ConsoleInterface::console_read(
    std::string& buffer, std::vector<char> break_sequence) const
{
    char c{0};

    std::size_t break_sequence_index = 0;

    while (std::cin.get(c)) {
        if (!break_sequence.empty()) {
            if (c == break_sequence[break_sequence_index]) {
                break_sequence_index++;
                if (break_sequence_index == break_sequence.size()) {
                    break;
                }
            }
            else {
                break_sequence_index = 0;
            }
        }
        buffer += c;
    }
}
}  // namespace hestia