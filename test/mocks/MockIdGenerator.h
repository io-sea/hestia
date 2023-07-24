#pragma once

#include "IdGenerator.h"

namespace hestia::mock {

class MockIdGenerator : public IdGenerator {
  public:
    std::string get_id(const std::string&) override
    {
        const auto id_str = std::to_string(m_id);
        m_id++;
        return id_str;
    }

    mutable unsigned m_id{1};
};
}  // namespace hestia::mock