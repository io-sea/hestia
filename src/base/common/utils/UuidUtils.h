#pragma once

#include "Uuid.h"

namespace hestia {
class UuidUtils {
  public:
    static Uuid from_string(
        const std::string& input,
        Uuid::Format format = Uuid::Format::Hex8_4_4_4_12,
        char delimiter      = '-');

    static std::string to_string(
        const Uuid& uuid,
        Uuid::Format format = Uuid::Format::Hex8_4_4_4_12,
        char delimiter      = '-');
};
}  // namespace hestia