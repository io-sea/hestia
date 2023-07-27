#pragma once

#include "Uuid.h"

#include <vector>

namespace hestia {
class UuidUtils {
  public:
    static Uuid from_string(
        const std::string& input,
        Uuid::Format format = Uuid::Format::Hex8_4_4_4_12,
        char delimiter      = '-');

    static Uuid from_bytes(const std::vector<unsigned char>& bytes);

    static std::string to_string(
        const Uuid& uuid,
        Uuid::Format format = Uuid::Format::Hex8_4_4_4_12,
        char delimiter      = '-');
};
}  // namespace hestia