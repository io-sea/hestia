#pragma once

#include <string>

namespace hestia {
class Datamover {
  public:
    Datamover(const std::string& host);

    ~Datamover();

    void apply_dataset_command(
        const std::string& dataset_id, const std::string& command);

    void apply_object_command(
        const std::string& id, const std::string& command);
};
}  // namespace hestia