#pragma once

#include "HsmObject.h"
#include "OwnableModel.h"

#include <string>
#include <vector>

namespace hestia {
class Dataset : public OwnableModel {
  public:
    Dataset();

    Dataset(const std::string& name);

    Dataset(const Uuid& id);

    void deserialize(
        const Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL) override;

    void serialize(
        Dictionary& dict,
        SerializeFormat format = SerializeFormat::FULL,
        const Uuid& id         = {}) const override;

    const std::vector<HsmObject>& objects() const { return m_objects; }

  private:
    std::vector<HsmObject> m_objects;
};
}  // namespace hestia