#pragma once

#include "HsmObject.h"
#include "OwnableModel.h"

#include <string>
#include <unordered_map>

namespace hestia {
class Namespace : public OwnableModel {
  public:
    Namespace();

    Namespace(const std::string& name);

    Namespace(const Uuid& id);

    void deserialize(
        const Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL) override;

    void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const override;

  private:
    Uuid m_dataset_id;
    std::unordered_map<std::string, Uuid> m_paths;
};
}  // namespace hestia