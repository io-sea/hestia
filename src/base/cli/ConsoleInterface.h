#pragma once

#include <string>
#include <vector>

namespace hestia {
class IConsoleInterface {
  public:
    virtual ~IConsoleInterface() = default;

    virtual void console_write(const std::string& output) const = 0;

    virtual void console_write(
        const std::vector<std::string>& output) const = 0;

    virtual void console_write_error(const std::string& output) const = 0;

    virtual void console_read(std::string& buffer) const = 0;
};

class ConsoleInterface : public IConsoleInterface {
  public:
    virtual ~ConsoleInterface() = default;

    void console_write(const std::string& output) const override;

    void console_write(const std::vector<std::string>& output) const override;

    void console_write_error(const std::string& output) const override;

    void console_read(std::string& buffer) const override;
};
}  // namespace hestia