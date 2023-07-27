#pragma once

#include <string>

namespace hestia {
class IdGenerator {
  public:
    virtual ~IdGenerator()                             = default;
    virtual std::string get_id(const std::string& key) = 0;
};

class DefaultIdGenerator : public IdGenerator {
  public:
    DefaultIdGenerator(uint64_t minimum_id = 0);

    std::string get_id(const std::string& key) override;

  private:
    uint64_t m_minimum_id{0};
};
}  // namespace hestia